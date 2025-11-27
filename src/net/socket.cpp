#include <span>
#include <utility>

#include <cstddef>

#include <sfap/net/proactor.hpp>
#include <sfap/net/socket.hpp>
#include <sfap/net/types.hpp>

sfap::net::Socket::Socket(sfap::net::Proactor* owner, socket_t handle) noexcept : owner_(owner), handle_(handle) {}

sfap::net::Socket::Socket(Socket&& other) noexcept
    : owner_(std::exchange(other.owner_, nullptr)), handle_(std::exchange(other.handle_, 0)) {}

sfap::net::Socket::~Socket() noexcept {
    if (owner_ && handle_ != 0) {
        owner_->close(handle_);
        owner_ = nullptr;
        owner_ = 0;
    }
}

sfap::net::Socket& sfap::net::Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        if (is_valid())
            owner_->close(handle_);
        owner_ = std::exchange(other.owner_, nullptr);
        handle_ = std::exchange(other.handle_, 0);
    }
    return *this;
}

bool sfap::net::Socket::is_valid() const noexcept {
    return owner_ != nullptr && handle_ != 0;
}

sfap::net::Socket::operator bool() const noexcept {
    return is_valid();
}

sfap::net::socket_t sfap::net::Socket::get_handle() const noexcept {
    return handle_;
}

sfap::task<void> sfap::net::Socket::send_bytes(std::span<const std::byte> data) noexcept {
    if (!is_valid() || data.empty())
        co_return;

    std::size_t offset = 0;
    while (offset < data.size()) {
        std::span<const std::byte> chunk = data.subspan(offset);
        const std::size_t sent = co_await owner_->socket_send(handle_, chunk);
        if (sent == 0)
            break;
        offset += sent;
    }

    co_return;
}

sfap::task<void> sfap::net::Socket::recv_bytes(std::span<std::byte> data, bool exact) noexcept {
    if (!is_valid() || data.empty())
        co_return;

    std::size_t offset = 0;
    while (offset < data.size()) {
        std::span<std::byte> chunk = data.subspan(offset);
        const std::size_t received = co_await owner_->socket_recv(handle_, chunk);
        if (received == 0)
            break;
        offset += received;
        if (!exact)
            break;
    }

    co_return;
}