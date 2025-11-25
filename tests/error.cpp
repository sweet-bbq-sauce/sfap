#include <gtest/gtest.h>

#include <cerrno>
#include <cstring>

#include <sfap/error.hpp>
#include <sfap/utils/expected.hpp>

using sfap::errc;
using sfap::error_code;
using sfap::generic_error;
using sfap::network_error;
using sfap::system_error;

static error_code get_error(sfap::unexpected<error_code> u) {
    return u.error();
}

TEST(ErrorCode, StoresCodeAndCategory) {
    struct dummy_cat : sfap::error_category {
        const char* name() const noexcept override {
            return "dummy";
        }
        const char* message(int) const noexcept override {
            return "msg";
        }
    };

    static const dummy_cat cat;

    error_code ec(123, cat);

    EXPECT_EQ(123, ec.code());
    EXPECT_STREQ("dummy", ec.name());
    EXPECT_STREQ("msg", ec.message());
    EXPECT_EQ(&cat, &ec.category());
}

TEST(ErrorCode, EqualitySameCodeAndCategory) {
    struct dummy_cat : sfap::error_category {
        const char* name() const noexcept override {
            return "dummy";
        }
        const char* message(int) const noexcept override {
            return "msg";
        }
    };

    static const dummy_cat cat1;
    static const dummy_cat cat2;

    error_code a(1, cat1);
    error_code b(1, cat1);
    error_code c(1, cat2);
    error_code d(2, cat1);

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_FALSE(a == d);
}

TEST(GenericError, InvalidArgumentHasGenericCategoryAndMessage) {
    auto u = generic_error(errc::INVALID_ARGUMENT);
    auto ec = get_error(u);

    EXPECT_STREQ("generic", ec.name());
    EXPECT_EQ(static_cast<int>(errc::INVALID_ARGUMENT), ec.code());
    EXPECT_STREQ("invalid argument", ec.message());
}

TEST(SystemError, WrapsErrnoAndSystemCategory) {
    errno = EINVAL;
    auto u = system_error();
    auto ec = get_error(u);

    EXPECT_STREQ("system", ec.name());
    EXPECT_EQ(EINVAL, ec.code());

    const char* msg = ec.message();
    ASSERT_NE(msg, nullptr);
    EXPECT_GT(std::strlen(msg), 0u);
}

TEST(SystemError, EqualityForSameErrno) {
    errno = EINVAL;
    auto e1 = get_error(system_error());
    errno = EINVAL;
    auto e2 = get_error(system_error());

    EXPECT_TRUE(e1 == e2);
}

TEST(NetworkError, UsesNetworkCategoryAndGivenCode) {
    constexpr int code = 1234;
    auto u = network_error(code);
    auto ec = get_error(u);

    EXPECT_STREQ("network", ec.name());
    EXPECT_EQ(code, ec.code());

    const char* msg = ec.message();
    ASSERT_NE(msg, nullptr);
    EXPECT_GT(std::strlen(msg), 0u);
}

TEST(NetworkError, EqualitySameCodeSameCategory) {
    auto e1 = get_error(network_error(5));
    auto e2 = get_error(network_error(5));
    auto e3 = get_error(network_error(6));
    auto s1 = get_error(system_error(5));

    EXPECT_TRUE(e1 == e2);
    EXPECT_FALSE(e1 == e3);
    EXPECT_FALSE(e1 == s1);
}
