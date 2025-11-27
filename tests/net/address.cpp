#include <optional>
#include <string_view>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <gtest/gtest.h>

#include <sfap/error.hpp>
#include <sfap/net/address.hpp>
#include <sfap/net/types.hpp>
#include <sfap/utils/string.hpp>

using sfap::errc;
using sfap::error_code;
using sfap::String;
using sfap::net::Address;
using sfap::net::ipx_t;
using sfap::net::port_t;

TEST(Address, DefaultConstructedIsEmpty) {
    Address addr;

    EXPECT_FALSE(static_cast<bool>(addr));
    EXPECT_FALSE(addr.is_bindable());
    EXPECT_FALSE(addr.is_connectable());

    auto internal = addr.get_address();
    EXPECT_FALSE(internal.has_value());

    auto origin = addr.get_origin();
    EXPECT_FALSE(origin.has_value());
}

TEST(Address, FromIpSetsAddressAndClearsOrigin) {
    Address addr;

    ipx_t ip{};
    addr.from_ip(ip);

    EXPECT_TRUE(static_cast<bool>(addr));
    EXPECT_TRUE(addr.is_bindable());
    EXPECT_FALSE(addr.is_connectable());

    auto internal = addr.get_address();
    ASSERT_TRUE(internal.has_value());
    EXPECT_EQ(0u, internal->port_);

    auto origin = addr.get_origin();
    EXPECT_FALSE(origin.has_value());
}

TEST(Address, FromHostnameInvalidDoesNotChangeState) {
    Address addr;

    error_code ec = addr.from_hostname("");
    EXPECT_EQ(static_cast<int>(errc::INVALID_ARGUMENT), ec.code());
    EXPECT_STREQ("generic", ec.name());

    EXPECT_FALSE(static_cast<bool>(addr));
    EXPECT_FALSE(addr.is_bindable());
    EXPECT_FALSE(addr.is_connectable());
    EXPECT_FALSE(addr.get_address().has_value());
    EXPECT_FALSE(addr.get_origin().has_value());
}

TEST(Address, FromHostnameSetsAddressForIpLiteralButNoOrigin) {
    Address addr;

    error_code ec = addr.from_hostname("127.0.0.1");
    EXPECT_EQ(0, ec.code());

    EXPECT_TRUE(static_cast<bool>(addr));
    EXPECT_TRUE(addr.is_bindable());

    auto internal = addr.get_address();
    ASSERT_TRUE(internal.has_value());
    EXPECT_EQ(0u, internal->port_);

    auto origin = addr.get_origin();
    EXPECT_FALSE(origin.has_value());
}

TEST(Address, FromHostnameSetsOriginForRealHostname) {
    Address addr;

    error_code ec = addr.from_hostname("localhost");
    EXPECT_EQ(0, ec.code());

    EXPECT_TRUE(static_cast<bool>(addr));
    EXPECT_TRUE(addr.is_bindable());

    auto origin = addr.get_origin();
    ASSERT_TRUE(origin.has_value());
    EXPECT_EQ(std::string_view("localhost"), origin.value());
}

TEST(Address, FromHostnameKeepsPortIfAlreadySet) {
    Address addr;

    const error_code ec1 = addr.from_hostname("127.0.0.1");
    EXPECT_EQ(0, ec1.code());
    addr.set_port(8080);

    const auto before = addr.get_address();
    ASSERT_TRUE(before.has_value());
    port_t previous_port = before->port_;

    const error_code ec2 = addr.from_hostname("127.0.0.1");
    EXPECT_EQ(0, ec2.code());

    const auto after = addr.get_address();
    ASSERT_TRUE(after.has_value());
    EXPECT_EQ(previous_port, after->port_);
}

TEST(Address, CtorFromHostnameBestEffort) {
    Address a(String("127.0.0.1"), 1234);
    EXPECT_TRUE(static_cast<bool>(a));
    EXPECT_TRUE(a.is_bindable());
    EXPECT_TRUE(a.is_connectable());

    auto internal = a.get_address();
    ASSERT_TRUE(internal.has_value());
    EXPECT_EQ(1234u, internal->port_);
}

TEST(Address, SetPortRequiresExistingAddress) {
    Address empty;
    empty.set_port(5555);

    EXPECT_FALSE(static_cast<bool>(empty));
    EXPECT_FALSE(empty.get_address().has_value());

    Address addr;
    addr.from_ip(ipx_t{});
    auto before = addr.get_address();
    ASSERT_TRUE(before.has_value());
    EXPECT_EQ(0u, before->port_);

    addr.set_port(4321);
    auto after = addr.get_address();
    ASSERT_TRUE(after.has_value());
    EXPECT_EQ(4321u, after->port_);
}

TEST(Address, ConnectableRequiresPortAndNonAnyIp) {
    Address addr;

    addr.from_ip(ipx_t{});
    EXPECT_TRUE(addr.is_bindable());
    EXPECT_FALSE(addr.is_connectable());

    addr.set_port(80);
    EXPECT_TRUE(addr.is_bindable());
    EXPECT_FALSE(addr.is_connectable());

    Address conn(String("127.0.0.1"), 8080);
    if (conn) {
        EXPECT_TRUE(conn.is_bindable());
        EXPECT_TRUE(conn.is_connectable());
    }
}

TEST(Address, ClearResetsState) {
    Address addr(String("127.0.0.1"), 8080);

    addr.clear();

    EXPECT_FALSE(static_cast<bool>(addr));
    EXPECT_FALSE(addr.is_bindable());
    EXPECT_FALSE(addr.is_connectable());
    EXPECT_FALSE(addr.get_address().has_value());
    EXPECT_FALSE(addr.get_origin().has_value());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

#ifdef _WIN32
    WSADATA wsaData{};
    int rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rc != 0) {
        std::fprintf(stderr, "WSAStartup failed: %d\n", rc);
        return rc;
    }
#endif

    int result = RUN_ALL_TESTS();

#ifdef _WIN32
    WSACleanup();
#endif

    return result;
}