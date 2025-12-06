#include <chrono>
#include <future>
#include <thread>

#include <gtest/gtest.h>

#include <sfap/net/platform/iouring.hpp>

using sfap::net::IOUringProactor;
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