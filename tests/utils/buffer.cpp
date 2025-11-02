#include <algorithm>
#include <array>
#include <optional>
#include <string>

#include <cstring>

#include <gtest/gtest.h>

#include <sfap/utils/buffer.hpp>

using sfap::Buffer;

static std::byte b(unsigned char v) {
    return static_cast<std::byte>(v);
}

static void fill_bytes(Buffer& buf, const std::string& s) {
    ASSERT_LE(s.size(), buf.capacity());
    ASSERT_TRUE(buf.resize(s.size()));
    std::memcpy(buf.data(), s.data(), s.size());
}

TEST(Buffer, CapacityZero) {
    Buffer b0{std::size_t{0}};
    EXPECT_FALSE(static_cast<bool>(b0));
    EXPECT_EQ(b0.capacity(), 0u);
    EXPECT_EQ(b0.size(), 0u);
    EXPECT_TRUE(b0.empty());
    EXPECT_EQ(b0.free(), 0u);
    EXPECT_EQ(b0.begin(), nullptr);
    EXPECT_EQ(b0.end(), nullptr);
    EXPECT_EQ(b0.view().size(), 0u);
    EXPECT_EQ(b0.view().data(), nullptr);
}

TEST(Buffer, CapacityNonZero) {
    Buffer b{8};
    EXPECT_TRUE(static_cast<bool>(b));
    EXPECT_EQ(b.capacity(), 8u);
    EXPECT_EQ(b.size(), 0u);
    EXPECT_TRUE(b.empty());
    EXPECT_FALSE(b.full());
    EXPECT_EQ(b.free(), 8u);
    EXPECT_NE(b.begin(), nullptr);
    EXPECT_EQ(b.begin(), b.end());
}

TEST(Buffer, Resize) {
    Buffer b{8};
    EXPECT_TRUE(b.resize(5));
    EXPECT_EQ(b.size(), 5u);
    EXPECT_EQ(b.free(), 3u);
    EXPECT_FALSE(b.full());
    EXPECT_TRUE(b.resize(8));
    EXPECT_TRUE(b.full());
    EXPECT_FALSE(b.resize(9));
    EXPECT_EQ(b.size(), 8u);
}

TEST(Buffer, IndexAndAt) {
    Buffer b{4};
    fill_bytes(b, "ABCD");

    EXPECT_EQ(static_cast<unsigned char>(b[0]), 'A');
    EXPECT_EQ(static_cast<unsigned char>(b[3]), 'D');

    auto v0 = b.at(0);
    ASSERT_TRUE(v0.has_value());
    EXPECT_EQ(static_cast<unsigned char>(*v0), 'A');

    EXPECT_FALSE(b.at(4).has_value());
    EXPECT_FALSE(b.at(100).has_value());
}

TEST(Buffer, Spans) {
    Buffer b{6};
    fill_bytes(b, "ABCDEF");
    auto v = b.view();
    auto cv = static_cast<const Buffer&>(b).view();

    ASSERT_EQ(v.size(), 6u);
    ASSERT_EQ(cv.size(), 6u);
    EXPECT_EQ(v.data(), b.data());
    EXPECT_EQ(cv.data(), b.data());
    EXPECT_TRUE(std::equal(v.begin(), v.end(), reinterpret_cast<const std::byte*>("ABCDEF")));
}

TEST(Buffer, SameContentEqual) {
    Buffer a{6}, c{6};
    fill_bytes(a, "ABCDEF");
    fill_bytes(c, "ABCDEF");

    EXPECT_TRUE(a == c);
    c.data()[2] = b('X');
    EXPECT_FALSE(a == c);
}

TEST(Buffer, SamePointerFastPath) {
    std::array<std::byte, 4> ext{};
    Buffer e1{std::span<std::byte>{ext.data(), ext.size()}};
    Buffer e2{std::span<std::byte>{ext.data(), ext.size()}};
    ASSERT_TRUE(e1.resize(4));
    ASSERT_TRUE(e2.resize(4));
    std::memset(ext.data(), 0x7F, ext.size());

    EXPECT_TRUE(e1 == e2);

    EXPECT_TRUE(e1 == e1);
}

TEST(Buffer, DifferentSizesNotEqual) {
    Buffer a{6}, b{6};
    fill_bytes(a, "ABCDEF");
    fill_bytes(b, "ABCDE");
    EXPECT_FALSE(a == b);
}

TEST(Buffer, CtorAndAssign) {
    Buffer a{8};
    fill_bytes(a, "ABCDEFGH");
    auto* ptr = a.data();

    Buffer b{std::move(a)};
    EXPECT_EQ(b.capacity(), 8u);
    EXPECT_EQ(b.size(), 8u);
    EXPECT_EQ(b.data(), ptr);
    EXPECT_FALSE(static_cast<bool>(a));
    EXPECT_EQ(a.capacity(), 0u);
    EXPECT_EQ(a.size(), 0u);
    EXPECT_EQ(a.begin(), nullptr);
    EXPECT_EQ(a.end(), nullptr);

    Buffer c{4};
    fill_bytes(c, "WXYZ");
    c = std::move(b);
    EXPECT_EQ(c.capacity(), 8u);
    EXPECT_EQ(c.size(), 8u);
    EXPECT_EQ(c.data(), ptr);

    EXPECT_FALSE(static_cast<bool>(b));
    EXPECT_EQ(b.capacity(), 0u);
    EXPECT_EQ(b.size(), 0u);
}

TEST(Buffer, Clean) {
    Buffer b{5};
    fill_bytes(b, "HELLO");
    b.clean();
    EXPECT_EQ(b.size(), 0u);
    EXPECT_TRUE(b.empty());
    EXPECT_EQ(b.begin(), b.end());
}

TEST(Buffer, NonOwningLifetime) {
    std::array<std::byte, 3> ext{};
    {
        Buffer nb{std::span<std::byte>{ext.data(), ext.size()}};
        EXPECT_TRUE(static_cast<bool>(nb));
        EXPECT_EQ(nb.capacity(), ext.size());
        EXPECT_TRUE(nb.resize(3));
        nb[0] = b('X');
        nb[1] = b('Y');
        nb[2] = b('Z');
        EXPECT_EQ(static_cast<unsigned char>(ext[0]), 'X');
        EXPECT_EQ(static_cast<unsigned char>(ext[1]), 'Y');
        EXPECT_EQ(static_cast<unsigned char>(ext[2]), 'Z');
    }
}

TEST(Buffer, FindByte) {
    Buffer b{6};
    fill_bytes(b, "abcabc");
    auto p0 = b.find(std::byte{'a'}, 0);
    ASSERT_TRUE(p0.has_value());
    EXPECT_EQ(*p0, 0u);

    auto p1 = b.find(std::byte{'b'}, 2);
    ASSERT_TRUE(p1.has_value());
    EXPECT_EQ(*p1, 4u);

    auto p2 = b.find(std::byte{'z'}, 0);
    EXPECT_FALSE(p2.has_value());

    Buffer empty{0};
    EXPECT_FALSE(empty.find(std::byte{'a'}, 0).has_value());
}

TEST(Buffer, FindPatternBasic) {
    Buffer b{6};
    fill_bytes(b, "abcabc");
    auto p = b.find(std::span<const std::byte>{reinterpret_cast<const std::byte*>("cab"), 3}, 0);
    ASSERT_TRUE(p.has_value());
    EXPECT_EQ(*p, 2u);
}

TEST(Buffer, PatternEdgeCases) {
    Buffer b{6};
    fill_bytes(b, "abcabc");

    EXPECT_FALSE(b.find(std::span<const std::byte>{reinterpret_cast<const std::byte*>("ab"), 2}, 6).has_value());

    EXPECT_FALSE(b.find(std::span<const std::byte>{}, 0).has_value());

    EXPECT_FALSE(
        b.find(std::span<const std::byte>{reinterpret_cast<const std::byte*>("abcdefghijkl"), 12}, 0).has_value());

    EXPECT_FALSE(b.find(std::span<const std::byte>{reinterpret_cast<const std::byte*>("abc"), 3}, 4).has_value());
}

TEST(Buffer, FullEmptyFree) {
    Buffer b{3};
    EXPECT_TRUE(b.empty());
    EXPECT_FALSE(b.full());
    EXPECT_EQ(b.free(), 3u);

    fill_bytes(b, "AB");
    EXPECT_FALSE(b.empty());
    EXPECT_FALSE(b.full());
    EXPECT_EQ(b.free(), 1u);

    EXPECT_TRUE(b.resize(3));
    EXPECT_TRUE(b.full());
    EXPECT_EQ(b.free(), 0u);
}
