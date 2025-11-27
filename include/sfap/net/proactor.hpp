#pragma once

#include <chrono>
#include <span>

#include <sfap/net/address.hpp>
#include <sfap/net/types.hpp>
#include <sfap/utils/task.hpp>

namespace sfap::net {

class Socket;

class Proactor {
  public:
    using clock = std::chrono::steady_clock;
    using duration = clock::duration;
    using time_point = clock::time_point;

    virtual ~Proactor() = default;

    virtual void run() noexcept = 0;
    virtual void stop() noexcept = 0;

    virtual sfap::task<Socket> connect(const Address& address, duration timeout = duration::max()) noexcept = 0;
    virtual void close(socket_t id) noexcept = 0;
    virtual sfap::task<void> sleep_for(duration d) noexcept = 0;
    virtual sfap::task<std::size_t> socket_send(socket_t id, std::span<const std::byte> data) noexcept = 0;
    virtual sfap::task<std::size_t> socket_recv(socket_t id, std::span<std::byte> data) noexcept = 0;
};

} // namespace sfap::net