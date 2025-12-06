#pragma once

#include <sfap/config.hpp>

#if defined(SUPPORTED_IOURING)

#include <atomic>
#include <coroutine>
#include <span>
#include <unordered_map>

#include <cstddef>
#include <cstdint>

#include <liburing.h>

#include <sfap/error.hpp>
#include <sfap/net/address.hpp>
#include <sfap/net/proactor.hpp>
#include <sfap/net/socket.hpp>
#include <sfap/net/types.hpp>
#include <sfap/utils/task.hpp>

namespace sfap::net {

class IOUringProactor final : public Proactor {

  public:
    explicit IOUringProactor(std::size_t entries) noexcept;
    ~IOUringProactor() noexcept;

    IOUringProactor(const IOUringProactor&) = delete;
    IOUringProactor& operator=(const IOUringProactor&) = delete;

    operator bool() const noexcept override;
    error_code get_error() const noexcept override;

    void run() noexcept override;
    void stop() noexcept override;

    task<sfap::result<Socket>> connect(const Address& address, duration timeout = duration::max()) noexcept override;
    void close(socket_t handle) noexcept override;

    task<error_code> sleep_for(duration d) noexcept override;

    task<result<std::size_t>> socket_send(socket_t handle, std::span<const std::byte> data) noexcept override;
    task<result<std::size_t>> socket_recv(socket_t handle, std::span<std::byte> data) noexcept override;

  private:
    error_code last_error_{no_error()};

    struct SocketState {
        int handle{-1};
        bool closing{false};
    };

    enum class OperationType : std::uint8_t { CONNECT, SEND, RECV, TIMEOUT };

    class Awaiter;

    struct OperationData {
        OperationType type;
        socket_t handle;
        std::coroutine_handle<> coro;
        Awaiter* awaiter{};
    };

    class Awaiter {
      public:
        Awaiter(IOUringProactor& self, socket_t socket) noexcept;
        virtual ~Awaiter() = default;

        virtual void on_complete(int result) noexcept = 0;

        error_code get_error() const noexcept;

      protected:
        IOUringProactor& self_;
        socket_t socket_;
        error_code error_{};
        OperationData* operation_{};
    };

    io_uring ring_{};
    std::atomic_bool running_{false};

    socket_t next_handle_id_{1};
    std::unordered_map<socket_t, SocketState> sockets_;

    result<OperationData*> alloc_opdata() noexcept;
    void free_opdata(OperationData* opdata) noexcept;

    void handle_cqe(io_uring_cqe* cqe) noexcept;
};

} // namespace sfap::net

#endif