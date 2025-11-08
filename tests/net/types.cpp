#include <gtest/gtest.h>

#include <sfap/net/types.hpp>

using namespace sfap::net;

TEST(IPX, DefaultIsIPv4) {
    const ipx_t ip;

    EXPECT_TRUE(ip.is_4());
    EXPECT_EQ(ip.family(), ipx_t::Family::V4);
    EXPECT_EQ(ip.size(), 4u);
    EXPECT_NE(ip.data(), nullptr);
}

TEST(IPX, ConstructIPv4) {
    const ip4_t addr = {192, 168, 0, 1};
    const ipx_t ip(addr);

    EXPECT_TRUE(ip.is_4());
    EXPECT_FALSE(ip.is_6());
    EXPECT_EQ(ip.family(), ipx_t::Family::V4);
    EXPECT_EQ(ip.get_4(), addr);
    EXPECT_EQ(ip.size(), 4u);
    EXPECT_EQ(ip.data()[0], 192);
}

TEST(IPX, ConstructIPv6) {
    const ip6_t addr = {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    const ipx_t ip(addr);

    EXPECT_TRUE(ip.is_6());
    EXPECT_FALSE(ip.is_4());
    EXPECT_EQ(ip.family(), ipx_t::Family::V6);
    EXPECT_EQ(ip.get_6(), addr);
    EXPECT_EQ(ip.size(), 16u);
    EXPECT_EQ(ip.data()[0], 0x20);
}

TEST(IPX, DataPointerAndSizeMatchFamily) {
    const ip4_t ipv4 = {1, 2, 3, 4};
    const ipx_t ip4(ipv4);
    EXPECT_EQ(ip4.size(), 4u);

    const ip6_t ipv6 = {0};
    const ipx_t ip6(ipv6);
    EXPECT_EQ(ip6.size(), 16u);
}
