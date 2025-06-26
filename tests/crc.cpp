#include <gtest/gtest.h>

#include <utils/crc.hpp>


using namespace sfap;
using namespace sfap::utils;


// Empty hostname
TEST( CRC, Empty ) {

    EXPECT_EQ( CRC::data_incremental( 0, nullptr, 0 ), 0 );

}


// String
TEST( CRC, String ) {

    EXPECT_EQ( CRC::data( "sfapisthebest", 13 ), 0x5b32daad );

}