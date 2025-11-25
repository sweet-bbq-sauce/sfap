#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

#include <gtest/gtest.h>

#include <sfap/net/resolve.hpp>
#include <sfap/net/types.hpp>
#include <sfap/utils/string.hpp>

using sfap::net::resolve;
using sfap::net::ResolveMode;

TEST(ResolveIntegrationTest, NullptrAddressReturnsNoname) {
    auto res = resolve(static_cast<const char*>(nullptr), ResolveMode::PREFER_IPV4);
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error().code(), EAI_NONAME);
}

TEST(ResolveIntegrationTest, LocalhostPreferIPv4ReturnsSomething) {
    auto res = resolve("localhost", ResolveMode::PREFER_IPV4);
    ASSERT_TRUE(res.has_value());
    EXPECT_TRUE(res->is_4() || res->is_6());
}

TEST(ResolveIntegrationTest, RequireIPv4OnLoopbackV4) {
    auto res = resolve("127.0.0.1", ResolveMode::REQUIRE_IPV4);
    ASSERT_TRUE(res.has_value());
    EXPECT_TRUE(res->is_4());
}

TEST(ResolveIntegrationTest, RequireIPv6OnLoopbackV6) {
    auto res = resolve("::1", ResolveMode::REQUIRE_IPV6);
    if (!res.has_value() && res.error().code() == EAI_NONAME)
        GTEST_SKIP() << "System has no IPv6 localhost (::1)";
    ASSERT_TRUE(res.has_value());
    EXPECT_TRUE(res->is_6());
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