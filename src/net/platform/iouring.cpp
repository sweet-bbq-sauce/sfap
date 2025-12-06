#include <sfap/config.hpp>

#if defined(SUPPORTED_IOURING)

#include <atomic>
#include <chrono>
#include <coroutine>
#include <span>
#include <unordered_map>

#include <cstddef>
#include <cstring>

#include <arpa/inet.h>
#include <fcntl.h>
#include <liburing.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <sfap/error.hpp>
#include <sfap/net/address.hpp>
#include <sfap/net/platform/iouring.hpp>
#include <sfap/net/proactor.hpp>
#include <sfap/net/socket.hpp>
#include <sfap/net/types.hpp>
#include <sfap/utils/expected.hpp>
#include <sfap/utils/task.hpp>

sfap::net::IOUringProactor::Awaiter::Awaiter(sfap::net::IOUringProactor& self, socket_t socket) noexcept
    : self_(self), socket_(socket) {}

sfap::error_code sfap::net::IOUringProactor::Awaiter::get_error() const noexcept {
    return error_;
}

sfap::net::IOUringProactor::IOUringProactor(std::size_t entries) noexcept {
    if (const int result = io_uring_queue_init(entries, &ring_, 0); result < 0)
        last_error_ = network_error(-result).error();
}

sfap::net::IOUringProactor::~IOUringProactor() noexcept {
    running_.store(false, std::memory_order_relaxed);

    for (auto& [sid, st] : sockets_) {
        if (st.handle >= 0) {
            ::close(st.handle);
            st.handle = -1;
        }
    }
    sockets_.clear();

    io_uring_queue_exit(&ring_);
}

sfap::net::IOUringProactor::operator bool() const noexcept {
    return !last_error_;
}

sfap::error_code sfap::net::IOUringProactor::get_error() const noexcept {
    return last_error_;
}

sfap::result<sfap::net::IOUringProactor::OperationData*> sfap::net::IOUringProactor::alloc_opdata() noexcept {
    auto* opdata{new (std::nothrow) OperationData{}};
    if (!opdata)
        return generic_error(errc::NOT_ENOUGH_MEMORY);
    return opdata;
}

void sfap::net::IOUringProactor::free_opdata(sfap::net::IOUringProactor::OperationData* opdata) noexcept {
    delete opdata;
}

void sfap::net::IOUringProactor::run() noexcept {
    running_.store(true, std::memory_order_release);

    while (running_.load(std::memory_order_acquire)) {
        io_uring_cqe* cqe = nullptr;
        if (const int result = io_uring_wait_cqe(&ring_, &cqe); result < 0)
            continue;

        handle_cqe(cqe);
        io_uring_cqe_seen(&ring_, cqe);
    }
}

void sfap::net::IOUringProactor::stop() noexcept {
    running_.store(false, std::memory_order_release);

    io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
    if (sqe) {
        io_uring_prep_nop(sqe);
        io_uring_sqe_set_data(sqe, nullptr);
        io_uring_submit(&ring_);
    }
}

sfap::task<sfap::result<sfap::net::Socket>> sfap::net::IOUringProactor::connect(const sfap::net::Address& address,
                                                                                duration) noexcept {
    const auto addr{address.get_address()};
    const int family{addr->ip_.is_4() ? AF_INET : AF_INET6};
    const int fd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (fd < 0)
        co_return network_error();

    sockaddr_storage ss{};
    const socket_t sid = next_handle_id_++;

    {
        ss.ss_family = family;
        void* destination;
        if (family == AF_INET) {
            auto* ipv4 = reinterpret_cast<sockaddr_in*>(&ss);
            destination = &ipv4->sin_addr;
            ipv4->sin_port = ::htons(addr->port_);
        } else {
            auto* ipv6 = reinterpret_cast<sockaddr_in6*>(&ss);
            destination = &ipv6->sin6_addr;
            ipv6->sin6_port = ::htons(addr->port_);
        }

        std::memcpy(destination, addr->ip_.data(), addr->ip_.size());
    }

    sockets_.emplace(sid, SocketState{fd, false});

    class ConnectAwaiter final : public Awaiter {

      public:
        explicit ConnectAwaiter(IOUringProactor& self_, socket_t socket_, sockaddr_storage* ss)
            : Awaiter(self_, socket_), address(ss) {}

        bool await_ready() const noexcept {
            return false;
        }

        void await_suspend(std::coroutine_handle<> h) noexcept {
            const auto it{self_.sockets_.find(socket_)};
            if (it == self_.sockets_.end()) {
                error_ = network_error().error();
                h.resume();
                return;
            }

            const int fd{it->second.handle};
            io_uring_sqe* sqe{io_uring_get_sqe(&self_.ring_)};
            if (!sqe) {
                error_ = network_error().error();
                h.resume();
                return;
            }

            const auto alloc_result{self_.alloc_opdata()};
            if (!alloc_result) {
                error_ = alloc_result.error();
                h.resume();
                return;
            }

            operation_ = *alloc_result;
            operation_->awaiter = this;
            operation_->type = OperationType::CONNECT;
            operation_->handle = socket_;
            operation_->coro = h;

            io_uring_prep_connect(sqe, fd, reinterpret_cast<const sockaddr*>(address),
                                  address->ss_family == AF_INET ? sizeof(sockaddr_in) : sizeof(sockaddr_in6));
            io_uring_sqe_set_data(sqe, operation_);

            if (const int result = io_uring_submit(&self_.ring_); result < 0) {
                self_.free_opdata(operation_);
                error_ = network_error(-result).error();
                h.resume();
            }
        }

        result<Socket> await_resume() noexcept {
            if (error_)
                return sfap::unexpected(error_);
            return Socket{&self_, socket_};
        }

        void on_complete(int result) noexcept override {
            if (result < 0)
                error_ = network_error(-result).error();
            else
                error_ = no_error();
        }

      private:
        sockaddr_storage* address;
    };

    ConnectAwaiter aw(*this, sid, &ss);
    auto result = co_await aw;

    const auto cleanup = [this, sid]() {
        const auto it = sockets_.find(sid);
        if (it != sockets_.end()) {
            if (it->second.handle >= 0)
                ::close(it->second.handle);
            sockets_.erase(it);
        }
    };

    if (!result) {
        cleanup();
        co_return sfap::unexpected(result.error());
    }

    if (!result->is_valid())
        cleanup();

    co_return result;
}

void sfap::net::IOUringProactor::close(sfap::net::socket_t handle) noexcept {
    const auto it = sockets_.find(handle);
    if (it == sockets_.end())
        return;

    SocketState& st = it->second;
    if (!st.closing && st.handle >= 0) {
        st.closing = true;
        ::close(st.handle);
        st.handle = -1;
    }
    sockets_.erase(it);
}

sfap::task<sfap::error_code> sfap::net::IOUringProactor::sleep_for(sfap::net::Proactor::duration d) noexcept {
    if (d <= duration::zero())
        co_return no_error();

    struct SleepAwaiter final : public Awaiter {

      public:
        explicit SleepAwaiter(IOUringProactor& self_, std::chrono::nanoseconds ns_) : Awaiter(self_, 0), ns(ns_) {}

        bool await_ready() const noexcept {
            return false;
        }

        void await_suspend(std::coroutine_handle<> h) noexcept {
            io_uring_sqe* sqe = io_uring_get_sqe(&self_.ring_);
            if (!sqe) {
                error_ = network_error().error();
                h.resume();
                return;
            }

            const auto alloc_result{self_.alloc_opdata()};
            if (!alloc_result) {
                error_ = alloc_result.error();
                h.resume();
                return;
            }

            operation_ = *alloc_result;
            operation_->awaiter = this;
            operation_->type = OperationType::TIMEOUT;
            operation_->handle = 0;
            operation_->coro = h;

            __kernel_timespec ts{};
            ts.tv_sec = static_cast<time_t>(ns.count() / 1'000'000'000);
            ts.tv_nsec = static_cast<long>(ns.count() % 1'000'000'000);

            io_uring_prep_timeout(sqe, &ts, 0, 0);
            io_uring_sqe_set_data(sqe, operation_);

            if (const int result = io_uring_submit(&self_.ring_); result < 0) {
                self_.free_opdata(operation_);
                error_ = network_error(-result).error();
                h.resume();
            }
        }

        void on_complete(int) noexcept override {
            error_ = no_error();
        }

        void await_resume() noexcept {}

      private:
        std::chrono::nanoseconds ns;
    };

    SleepAwaiter aw{*this, std::chrono::duration_cast<std::chrono::nanoseconds>(d)};
    co_await aw;
    co_return aw.get_error();
}

sfap::task<sfap::result<std::size_t>>
sfap::net::IOUringProactor::socket_send(socket_t sid, std::span<const std::byte> data) noexcept {
    struct SendAwaiter final : public Awaiter {
      public:
        explicit SendAwaiter(IOUringProactor& self, socket_t socket, std::span<const std::byte> data) noexcept
            : Awaiter(self, socket), data_(data) {}

        bool await_ready() const noexcept {
            return data_.empty();
        }

        void await_suspend(std::coroutine_handle<> h) noexcept {
            const auto it = self_.sockets_.find(socket_);
            if (it == self_.sockets_.end() || data_.empty()) {
                h.resume();
                return;
            }

            const int handle = it->second.handle;
            io_uring_sqe* sqe = io_uring_get_sqe(&self_.ring_);
            if (!sqe) {
                h.resume();
                return;
            }

            const auto alloc_result{self_.alloc_opdata()};
            if (!alloc_result) {
                error_ = alloc_result.error();
                h.resume();
                return;
            }

            operation_ = *alloc_result;
            operation_->awaiter = this;
            operation_->type = OperationType::SEND;
            operation_->handle = socket_;
            operation_->coro = h;

            io_uring_prep_send(sqe, handle, data_.data(), static_cast<size_t>(data_.size()), 0);
            io_uring_sqe_set_data(sqe, operation_);

            if (const int result = io_uring_submit(&self_.ring_); result < 0) {
                self_.free_opdata(operation_);
                error_ = network_error(-result).error();
                h.resume();
            }
        }

        result<std::size_t> await_resume() noexcept {
            if (error_)
                return sfap::unexpected(error_);
            return bytes_;
        }

        void on_complete(int result) noexcept override {
            if (result < 0) {
                error_ = network_error(-result).error();
                bytes_ = 0;
            } else {
                error_ = no_error();
                bytes_ = static_cast<std::size_t>(result);
            }
        }

      private:
        std::span<const std::byte> data_;
        std::size_t bytes_{};
    };

    SendAwaiter aw{*this, sid, data};
    co_return co_await aw;
}

sfap::task<sfap::result<std::size_t>> sfap::net::IOUringProactor::socket_recv(socket_t sid,
                                                                              std::span<std::byte> data) noexcept {
    struct RecvAwaiter final : public Awaiter {
      public:
        explicit RecvAwaiter(IOUringProactor& self, socket_t socket, std::span<std::byte> data) noexcept
            : Awaiter(self, socket), data_(data) {}

        std::span<std::byte> data_;
        std::size_t bytes_{};

        bool await_ready() const noexcept {
            return data_.empty();
        }

        void await_suspend(std::coroutine_handle<> h) noexcept {
            const auto it = self_.sockets_.find(socket_);
            if (it == self_.sockets_.end() || data_.empty()) {
                h.resume();
                return;
            }

            const int handle = it->second.handle;
            io_uring_sqe* sqe = io_uring_get_sqe(&self_.ring_);
            if (!sqe) {
                h.resume();
                return;
            }

            const auto alloc_result{self_.alloc_opdata()};
            if (!alloc_result) {
                error_ = alloc_result.error();
                h.resume();
                return;
            }

            operation_ = *alloc_result;
            operation_->awaiter = this;
            operation_->type = OperationType::RECV;
            operation_->handle = socket_;
            operation_->coro = h;

            io_uring_prep_recv(sqe, handle, data_.data(), static_cast<size_t>(data_.size()), 0);
            io_uring_sqe_set_data(sqe, operation_);

            if (const int result = io_uring_submit(&self_.ring_); result < 0) {
                self_.free_opdata(operation_);
                error_ = network_error(-result).error();
                h.resume();
            }
        }

        result<std::size_t> await_resume() noexcept {
            if (error_)
                return sfap::unexpected(error_);
            return bytes_;
        }

        void on_complete(int result) noexcept override {
            if (result < 0) {
                error_ = network_error(-result).error();
                bytes_ = 0;
            } else {
                error_ = no_error();
                bytes_ = static_cast<std::size_t>(result);
            }
        }
    };

    RecvAwaiter aw{*this, sid, data};
    co_return co_await aw;
}

void sfap::net::IOUringProactor::handle_cqe(io_uring_cqe* cqe) noexcept {
    auto* operation = static_cast<OperationData*>(io_uring_cqe_get_data(cqe));
    if (!operation)
        return;

    const int result{cqe->res};
    Awaiter* aw{operation->awaiter};

    if (aw)
        aw->on_complete(result);
    auto h = operation->coro;
    free_opdata(operation);
    if (h)
        h.resume();
}

#endif