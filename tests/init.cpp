#include <gtest/gtest.h>

#include <sfap/test.hpp>


TEST( Init, Test ) {

    ASSERT_EQ( sfap::hello(), "Hello!" );

}