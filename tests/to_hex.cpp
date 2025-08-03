#include <vector>
#include <string>

#include <gtest/gtest.h>

#include <sfap.hpp>
#include <utils/to_hex.hpp>


using namespace sfap;
using namespace sfap::utils;


TEST( ToHex, EmptyBuffer ) {

    std::vector<byte_t> data;
    EXPECT_EQ( to_hex( data ), "" );

}


TEST( ToHex, SimpleValuesLowercase ) {

    set_hex_format_mode( HexFormatMode::LOWERCASE );

    std::vector<byte_t> data = {0x00, 0x0A, 0xFF};
    EXPECT_EQ( to_hex( data ), "000aff" );
}


TEST( ToHex, SimpleValuesUppercase ) {

    set_hex_format_mode( HexFormatMode::UPPERCASE );

    std::vector<byte_t> data = {0x00, 0x0A, 0xFF};
    EXPECT_EQ( to_hex( data ), "000AFF" );

}


TEST( ToHex, StringConversion ) {

    set_hex_format_mode( HexFormatMode::LOWERCASE );

    const std::string str = "AB";

    // ASCII 'A' = 0x41, 'B' = 0x42
    EXPECT_EQ( to_hex(str), "4142" );

}


TEST( ToHex, ObjectConversion ) {

    set_hex_format_mode( HexFormatMode::UPPERCASE );

    uint16_t value = 0x1234;
    std::string hex = to_hex_object( value );

    ASSERT_EQ( hex.size(), 4 );

    for ( const char c : hex ) {

        EXPECT_TRUE( std::string( "0123456789ABCDEF" ).find( c ) != std::string::npos );

    }
}
