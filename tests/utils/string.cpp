#include <string>
#include <string_view>

#include <cstring>

#include <gtest/gtest.h>

#include <sfap/utils/string.hpp>

using sfap::String;

TEST(String, ConstructsFromEmpty) {
    String s(std::string_view{});

    EXPECT_TRUE(static_cast<bool>(s));
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0u);
    ASSERT_NE(s.c_str(), nullptr);
    EXPECT_STREQ(s.c_str(), "");
    EXPECT_TRUE(s.view().empty());
    EXPECT_EQ(*(s.c_str() + s.size()), '\0');
}

TEST(String, ConstructFronNull) {
    String s{nullptr};

    EXPECT_FALSE(s);
    EXPECT_STREQ(s.c_str(), "");
}

TEST(String, CopyCtor) {
    String a("abc");
    String b(a);
    EXPECT_EQ(b.size(), 3u);
    EXPECT_STREQ(b.c_str(), "abc");
    EXPECT_EQ(*(b.c_str() + b.size()), '\0');
}

TEST(String, CString) {
    String s("cstring");

    EXPECT_TRUE(s);
    EXPECT_EQ(s.size(), 7u);
    EXPECT_STREQ(s.c_str(), "cstring");
}

TEST(String, View) {
    String s("giggity");
    const auto view = s.view();

    EXPECT_EQ(std::string{view}, "giggity");
}