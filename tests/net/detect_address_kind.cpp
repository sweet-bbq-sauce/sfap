#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include <sfap/net/address_kind.hpp>
#include <sfap/net/types.hpp>
#include <sfap/utils/string.hpp>

using sfap::String;
using sfap::net::AddressKind;
using sfap::net::detect_address_kind;

TEST(detect_address_kind, Empty) {
    String addr(std::string_view{});
    const auto r = detect_address_kind(addr);

    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(*r, AddressKind::EMPTY);
}

TEST(detect_address_kind, NullString) {
    const auto r = detect_address_kind(nullptr);

    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(*r, AddressKind::EMPTY);
}

TEST(detect_address_kind, IPv4_Valid) {
    String addr("203.0.113.5");
    const auto r = detect_address_kind(addr);

    ASSERT_TRUE(r);
    EXPECT_EQ(*r, AddressKind::IP4);
}

TEST(detect_address_kind, IPv4_Invalid) {
    String addr("999.1.1.1");
    const auto r = detect_address_kind(addr);

    ASSERT_TRUE(r);
    EXPECT_EQ(*r, AddressKind::UNKNOWN);
}

TEST(detect_address_kind, IPv6_Valid_Short) {
    String addr("::1");
    const auto r = detect_address_kind(addr);

    ASSERT_TRUE(r);
    EXPECT_EQ(*r, AddressKind::IP6);
}

TEST(detect_address_kind, IPv6_Invalid) {
    String addr("gggg::1");
    const auto r = detect_address_kind(addr);

    ASSERT_TRUE(r);
    EXPECT_EQ(*r, AddressKind::UNKNOWN);
}

TEST(detect_address_kind, Hostname_Valid) {
    String addr("example.com");
    const auto r = detect_address_kind(addr);

    ASSERT_TRUE(r);
    EXPECT_EQ(*r, AddressKind::HOSTNAME);
}

TEST(detect_address_kind, Hostname_Valid_TrailingDot) {
    String addr("example.com.");
    const auto r = detect_address_kind(addr);

    ASSERT_TRUE(r);
    EXPECT_EQ(*r, AddressKind::HOSTNAME);
}

TEST(detect_address_kind, Hostname_Punycode_TLD) {
    String addr("xn--d1acpjx3f.xn--p1ai");
    const auto r = detect_address_kind(addr);

    ASSERT_TRUE(r);
    EXPECT_EQ(*r, AddressKind::HOSTNAME);
}

TEST(detect_address_kind, Hostname_LabelTooLong) {
    const std::string label(64, 'a'); // 64 > 63
    const std::string fqdn = label + ".com";
    String addr(fqdn);
    const auto r = detect_address_kind(addr);

    ASSERT_TRUE(r);
    EXPECT_EQ(*r, AddressKind::UNKNOWN);
}

TEST(detect_address_kind, Hostname_TotalLengthTooLong) {
    std::string fqdn;
    while (fqdn.size() <= 254) {
        if (!fqdn.empty()) fqdn.push_back('.');
        fqdn += std::string(63, 'a');
    }

    String addr(fqdn);
    const auto r = detect_address_kind(addr);

    ASSERT_TRUE(r);
    EXPECT_EQ(*r, AddressKind::UNKNOWN);
}

TEST(detect_address_kind, IPv6_Brackets_Unknown) {
    String addr("[::1]");
    const auto r = detect_address_kind(addr);

    ASSERT_TRUE(r);
    EXPECT_EQ(*r, AddressKind::UNKNOWN);
}

TEST(detect_address_kind, IPv6_ZoneId_Unknown) {
    String addr("fe80::1%eth0");
    const auto r = detect_address_kind(addr);

    ASSERT_TRUE(r);
    EXPECT_EQ(*r, AddressKind::UNKNOWN);
}

TEST(detect_address_kind, EmbeddedNul_IPv4_TreatedAsIPv4) {
    const char raw[] = {'1','2','7','.','0','.','0','.','1','\0','x','y','z'};
    String addr(std::string_view{raw, sizeof(raw)});
    const auto r = detect_address_kind(addr);

    ASSERT_TRUE(r);
    EXPECT_EQ(*r, AddressKind::IP4);
}