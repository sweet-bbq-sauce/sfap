#include <sfap/config.hpp>

#if defined(SUPPORTED_IOURING)

#include <chrono>
#include <future>
#include <thread>

#include <cstring>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <gtest/gtest.h>

#include <sfap/net/platform/iouring.hpp>

using sfap::net::IOUringProactor;
using sfap::net::Socket;
using namespace std::chrono_literals;

TEST(IOUringProactor, Init) {
    IOUringProactor pro(2048);
    EXPECT_TRUE(pro);
    EXPECT_FALSE(pro.get_error());
}

TEST(IOUringProactor, SleepForCompletesAndReturnsNoError) {
    sfap::net::IOUringProactor proactor{256};

    if (!proactor) {
        GTEST_SKIP() << "io_uring not available: " << proactor.get_error().message();
    }

    std::thread loop([&] { proactor.run(); });

    std::promise<void> done;
    auto fut = done.get_future();

    auto coro = [&]() -> sfap::task<void> {
        auto start = std::chrono::steady_clock::now();

        sfap::error_code ec = co_await proactor.sleep_for(50ms);
        EXPECT_FALSE(ec) << "sleep_for returned error: " << ec.message();

        auto end = std::chrono::steady_clock::now();
        auto elapsed = end - start;
        EXPECT_GE(elapsed, 50ms);

        done.set_value();
        co_return;
    };

    auto task = coro();
    task.start_detached();

    fut.wait();

    proactor.stop();
    loop.join();
}

TEST(IOUringProactor, LoopbackConnectSendRecvUsingSocket) {
    IOUringProactor proactor{256};
    if (!proactor) {
        GTEST_SKIP() << "io_uring not available: " << proactor.get_error().message();
    }

    std::promise<std::uint16_t> port_promise;
    auto port_future = port_promise.get_future();

    std::thread server_thread([&] {
        int srv = ::socket(AF_INET, SOCK_STREAM, 0);
        ASSERT_GE(srv, 0) << "socket() failed";

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = 0;

        ASSERT_EQ(::bind(srv, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)), 0) << "bind() failed";
        ASSERT_EQ(::listen(srv, 1), 0) << "listen() failed";

        socklen_t len = sizeof(addr);
        ASSERT_EQ(::getsockname(srv, reinterpret_cast<sockaddr*>(&addr), &len), 0) << "getsockname() failed";

        std::uint16_t port = ntohs(addr.sin_port);
        port_promise.set_value(port);

        int client = ::accept(srv, nullptr, nullptr);
        ASSERT_GE(client, 0) << "accept() failed";

        std::array<char, 64> buf{};
        const ssize_t n = ::recv(client, buf.data(), buf.size(), 0);
        ASSERT_GT(n, 0) << "recv() failed";

        const ssize_t m = ::send(client, buf.data(), static_cast<size_t>(n), 0);
        ASSERT_EQ(m, n) << "echo send() failed";

        ::close(client);
        ::close(srv);
    });

    std::thread loop([&] { proactor.run(); });

    std::promise<void> done;
    auto done_future = done.get_future();

    auto client_coro = [&]() -> sfap::task<void> {
        const std::uint16_t port = port_future.get();
        sfap::net::Address addr{"127.0.0.1", port};

        auto conn_res = co_await proactor.connect(addr);
        EXPECT_TRUE(conn_res) << "connect failed: " << conn_res.error().message();

        Socket sock = std::move(*conn_res);
        EXPECT_TRUE(sock.is_valid());

        const char msg[] = "Hello There. General Kenobi.";
        std::array<std::byte, sizeof(msg)> send_buf{};
        std::memcpy(send_buf.data(), msg, sizeof(msg));

        co_await sock.send_bytes(send_buf);

        std::array<std::byte, sizeof(msg)> recv_buf{};
        co_await sock.recv_bytes(recv_buf, true);

        EXPECT_EQ(std::memcmp(recv_buf.data(), send_buf.data(), sizeof(msg)), 0);

        done.set_value();
        co_return;
    };

    auto task = client_coro();
    task.start_detached();

    done_future.wait();

    proactor.stop();
    loop.join();
    server_thread.join();
}

#endif