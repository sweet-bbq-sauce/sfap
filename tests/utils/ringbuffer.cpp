#include <atomic>
#include <barrier>
#include <thread>

#include <cstring>

#include <gtest/gtest.h>

#include <sfap/utils/ringbuffer.hpp>

using sfap::RingBuffer;

class RingBufferTest : public ::testing::Test {

  protected:
    static std::byte B(uint8_t v) {
        return static_cast<std::byte>(v);
    }
};

TEST_F(RingBufferTest, ConstructPowerOfTwo) {
    RingBuffer rb{8};

    ASSERT_TRUE(rb);
    EXPECT_EQ(rb.capacity(), 8u);
    EXPECT_TRUE(rb.empty());
    EXPECT_FALSE(rb.full());
}

TEST_F(RingBufferTest, ConstructNonPowerOfTwoIsInvalid) {
    RingBuffer rb{6};

    EXPECT_FALSE(rb);
    EXPECT_EQ(rb.capacity(), 0u);
    EXPECT_TRUE(rb.empty());
}

TEST_F(RingBufferTest, SizeFreeInvariant) {
    RingBuffer rb{8};

    auto inv = [&] {
        EXPECT_EQ(rb.size() + rb.free(), rb.capacity());
        EXPECT_EQ(rb.empty(), rb.size() == 0);
        EXPECT_EQ(rb.full(), rb.size() == rb.capacity());
    };

    inv();

    for (int i = 0; i < 5; i++) {
        ASSERT_TRUE(rb.put(B(i)));
    }
    inv();

    std::byte tmp{};
    for (int i = 0; i < 3; i++) {
        ASSERT_TRUE(rb.pop(tmp));
    }
    inv();
}

TEST_F(RingBufferTest, PutPopWraps) {
    RingBuffer rb{8};

    for (int i = 0; i < 6; i++)
        ASSERT_TRUE(rb.put(B(i)));
    std::byte x{};
    for (int i = 0; i < 6; i++) {
        ASSERT_TRUE(rb.pop(x));
        EXPECT_EQ(x, B(i));
    }
    for (int i = 0; i < 8; i++)
        ASSERT_TRUE(rb.put(B(i + 10)));
    EXPECT_TRUE(rb.full());
    for (int i = 0; i < 8; i++) {
        ASSERT_TRUE(rb.pop(x));
        EXPECT_EQ(x, B(i + 10));
    }
    EXPECT_TRUE(rb.empty());
}

TEST_F(RingBufferTest, PrepareCommitNoWrap) {
    RingBuffer rb{8};

    auto vw = rb.prepare_write(6);
    EXPECT_EQ(RingBuffer::view_size(vw), 6u);
    EXPECT_EQ(vw.second.size(), 0u);

    uint8_t v = 1;
    for (auto& b : vw.first)
        b = B(v++);
    EXPECT_EQ(rb.commit_write(6), 6u);

    auto vr = rb.prepare_read(6);
    EXPECT_EQ(RingBuffer::view_size(vr), 6u);
    EXPECT_EQ(vr.second.size(), 0u);
    v = 1;
    for (auto b : vr.first)
        EXPECT_EQ(b, B(v++));
    EXPECT_EQ(rb.commit_read(6), 6u);
    EXPECT_TRUE(rb.empty());
}

TEST_F(RingBufferTest, PrepareCommitWrap) {
    RingBuffer rb{8};

    auto w1 = rb.prepare_write(6);
    ASSERT_EQ(RingBuffer::view_size(w1), 6u);
    std::fill(w1.first.begin(), w1.first.end(), B(0xAA));
    rb.commit_write(6);
    std::byte tmp{};
    for (int i = 0; i < 6; i++)
        ASSERT_TRUE(rb.pop(tmp));

    auto w2 = rb.prepare_write(6);
    ASSERT_EQ(RingBuffer::view_size(w2), 6u);
    ASSERT_GT(w2.second.size(), 0u);
    uint8_t c = 1;
    for (auto& b : w2.first)
        b = B(c++);
    for (auto& b : w2.second)
        b = B(c++);
    rb.commit_write(6);

    auto r2 = rb.prepare_read(6);
    ASSERT_EQ(RingBuffer::view_size(r2), 6u);

    c = 1;

    for (auto b : r2.first)
        EXPECT_EQ(b, B(c++));
    for (auto b : r2.second)
        EXPECT_EQ(b, B(c++));

    rb.commit_read(6);
}

TEST_F(RingBufferTest, CommitLessThanPrepared) {
    RingBuffer rb{8};
    auto w = rb.prepare_write(6);
    ASSERT_EQ(RingBuffer::view_size(w), 6u);
    // zapisz tylko 4 bajty
    int written = 0;
    for (auto& b : w.first) {
        if (written == 4)
            break;
        b = B(0x1);
        ++written;
    }
    EXPECT_EQ(rb.commit_write(4), 4u);
    auto r = rb.prepare_read(8);
    EXPECT_EQ(RingBuffer::view_size(r), 4u);
    rb.commit_read(4);
    EXPECT_TRUE(rb.empty());
}

TEST_F(RingBufferTest, CleanResetsState) {
    RingBuffer rb{8};
    for (int i = 0; i < 5; i++)
        ASSERT_TRUE(rb.put(B(i)));
    rb.clean();
    EXPECT_TRUE(rb.empty());
    EXPECT_EQ(rb.size(), 0u);
    EXPECT_EQ(rb.free(), rb.capacity());
}

TEST_F(RingBufferTest, MoveCtorAndAssign) {
    RingBuffer a{8};
    for (int i = 0; i < 3; i++)
        ASSERT_TRUE(a.put(B(10 + i)));

    RingBuffer b{2};
    b = std::move(a);
    EXPECT_FALSE(a);
    EXPECT_TRUE(b);

    std::byte x{};
    for (int i = 0; i < 3; i++) {
        ASSERT_TRUE(b.pop(x));
        EXPECT_EQ(x, B(10 + i));
    }
    EXPECT_TRUE(b.empty());

    RingBuffer c{8};
    for (int i = 0; i < 2; i++)
        ASSERT_TRUE(c.put(B(20 + i)));
    RingBuffer d{std::move(c)};
    EXPECT_FALSE(c);
    std::byte y{};
    for (int i = 0; i < 2; i++) {
        ASSERT_TRUE(d.pop(y));
        EXPECT_EQ(y, B(20 + i));
    }
}

TEST_F(RingBufferTest, SpscProducerConsumerMany) {
    RingBuffer rb{1024};
    const size_t N = 5'000'000;

    std::barrier start{2};
    std::atomic<bool> done{false};
    std::atomic<size_t> produced{0}, consumed{0};

    std::thread prod([&] {
        start.arrive_and_wait();
        for (size_t i = 0; i < N; i++) {
            while (true) {
                if (rb.put(B(static_cast<uint8_t>(i)))) {
                    produced++;
                    break;
                }
                auto vw = rb.prepare_write(64);
                if (RingBuffer::view_size(vw) == 0)
                    continue;
                size_t k = 0;
                for (auto& b : vw.first) {
                    b = B(static_cast<uint8_t>(i + k));
                    ++k;
                    if (i + k >= N)
                        break;
                }
                for (auto& b : vw.second) {
                    b = B(static_cast<uint8_t>(i + k));
                    ++k;
                    if (i + k >= N)
                        break;
                }
                rb.commit_write(k);
                produced += k;
                i += k - 1;
                break;
            }
        }
        done.store(true, std::memory_order_release);
    });

    std::thread cons([&] {
        start.arrive_and_wait();
        size_t i = 0;
        std::byte x{};
        while (true) {
            if (rb.pop(x)) {
                EXPECT_EQ(x, B(static_cast<uint8_t>(i)));
                ++i;
                ++consumed;
                continue;
            }
            auto vr = rb.prepare_read(64);
            auto m = RingBuffer::view_size(vr);
            if (m) {
                size_t k = 0;
                for (auto b : vr.first) {
                    EXPECT_EQ(b, B(static_cast<uint8_t>(i + k)));
                    ++k;
                }
                for (auto b : vr.second) {
                    EXPECT_EQ(b, B(static_cast<uint8_t>(i + k)));
                    ++k;
                }
                rb.commit_read(k);
                i += k;
                consumed += k;
                continue;
            }
            if (done.load(std::memory_order_acquire) && i == N)
                break;
        }
        EXPECT_EQ(i, N);
    });

    prod.join();
    cons.join();
    EXPECT_EQ(produced.load(), N);
    EXPECT_EQ(consumed.load(), N);
    EXPECT_TRUE(rb.empty());
}