#pragma once

#include <span>

#include <cstddef>

#include <sfap/net/types.hpp>
#include <sfap/utils/task.hpp>

namespace sfap::net {

class Proactor;

class Socket {
  public:
    Socket() noexcept = default;
    ~Socket() noexcept;

    explicit Socket(Proactor* owner, socket_t handle) noexcept;

    Socket(const Socket&) = default;
    Socket& operator=(const Socket&) = default;

    explicit Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    bool is_valid() const noexcept;
    explicit operator bool() const noexcept;

    socket_t get_handle() const noexcept;

    task<void> send_bytes(std::span<const std::byte> data) noexcept;
    task<void> recv_bytes(std::span<std::byte> data, bool exact = true) noexcept;

  private:
    friend class Proactor;

    Proactor* owner_{};
    socket_t handle_{};
};

} // namespace sfap::net