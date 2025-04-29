#include <gtest/gtest.h>

#include <net/address/detect.hpp>


// Empty hostname
TEST( Detect_Address_Type, Empty ) {

    EXPECT_EQ( (int)sfap::net::detect_address_type( "" ), (int)sfap::net::address_type::EMPTY );

}


// Valid RFC 1034 hostname
TEST( Detect_Address_Type, ValidHostname ) {

    EXPECT_EQ( (int)sfap::net::detect_address_type( "google.pl" ), (int)sfap::net::address_type::HOSTNAME );
    EXPECT_EQ( (int)sfap::net::detect_address_type( "gmail.google.pl" ), (int)sfap::net::address_type::HOSTNAME );
    EXPECT_EQ( (int)sfap::net::detect_address_type( "lol" ), (int)sfap::net::address_type::HOSTNAME );

}


TEST( Detect_Address_Type, InvalidHostname ) {

    EXPECT_EQ( (int)sfap::net::detect_address_type( "google.pl." ), (int)sfap::net::address_type::UNSUPPORTED );
    EXPECT_EQ( (int)sfap::net::detect_address_type( ".google.pl" ), (int)sfap::net::address_type::UNSUPPORTED );
    EXPECT_EQ( (int)sfap::net::detect_address_type( "/google.pl" ), (int)sfap::net::address_type::UNSUPPORTED );

    EXPECT_EQ( (int)sfap::net::detect_address_type( "asd@#!12" ), (int)sfap::net::address_type::UNSUPPORTED );
    EXPECT_EQ( (int)sfap::net::detect_address_type( "asd.@#!12.com" ), (int)sfap::net::address_type::UNSUPPORTED );

    EXPECT_EQ( (int)sfap::net::detect_address_type( "你好" ), (int)sfap::net::address_type::UNSUPPORTED );
    EXPECT_EQ( (int)sfap::net::detect_address_type( "你好.世界" ), (int)sfap::net::address_type::UNSUPPORTED );
    EXPECT_EQ( (int)sfap::net::detect_address_type( "你好.世界.com" ), (int)sfap::net::address_type::UNSUPPORTED );

    // Non-alphanumeric data in hostname
    EXPECT_EQ( (int)sfap::net::detect_address_type( ( []() {

        std::string hostname;

        for ( int i = 0; i < 10; i++ ) hostname.push_back( 0x24 );

        return hostname + ".aaa.bbb.com";

    })() ), (int)sfap::net::address_type::UNSUPPORTED );

    // Too long sub-domain
    EXPECT_EQ( (int)sfap::net::detect_address_type( ( []() {

        std::string hostname;

        for ( int i = 0; i < 64; i++ ) hostname.push_back( 'a' );

        return hostname + ".aaa.bbb.com";

    })() ), (int)sfap::net::address_type::UNSUPPORTED );

    // Too long whole domain
    EXPECT_EQ( (int)sfap::net::detect_address_type( ( []() {

        std::string hostname;

        for ( int i = 0; i < 255; i++ ) hostname.push_back( 'a' );

        return hostname;

    })() ), (int)sfap::net::address_type::UNSUPPORTED );

    // Null-filled hostname
    EXPECT_EQ( (int)sfap::net::detect_address_type( ( []() {

        std::string hostname;

        for ( int i = 0; i < 10; i++ ) hostname.push_back( 0x00 );

        return hostname;

    })() ), (int)sfap::net::address_type::UNSUPPORTED );

}


// Valid IPv4
TEST( Detect_Address_Type, ValidIPv4 ) {

    EXPECT_EQ( (int)sfap::net::detect_address_type( "127.0.0.0" ), (int)sfap::net::address_type::IPV4 );
    EXPECT_EQ( (int)sfap::net::detect_address_type( "127.255.255.255" ), (int)sfap::net::address_type::IPV4 );

}


// Invalid IPv4
TEST( Detect_Address_Type, InvalidIPv4 ) {

    EXPECT_EQ( (int)sfap::net::detect_address_type( "127.00.00.0.0" ), (int)sfap::net::address_type::HOSTNAME );
    EXPECT_EQ( (int)sfap::net::detect_address_type( ".127.0.0.0" ), (int)sfap::net::address_type::UNSUPPORTED );
    EXPECT_EQ( (int)sfap::net::detect_address_type( "127.0.0.0." ), (int)sfap::net::address_type::UNSUPPORTED );
    EXPECT_EQ( (int)sfap::net::detect_address_type( "127.33.43,34" ), (int)sfap::net::address_type::UNSUPPORTED );

}


// Invalid IPv4 but valid hostname
TEST( Detect_Address_Type, InvalidIPv4ButValidHostname ) {

    EXPECT_EQ( (int)sfap::net::detect_address_type( "127.00.0" ), (int)sfap::net::address_type::HOSTNAME );
    EXPECT_EQ( (int)sfap::net::detect_address_type( "127.33.434.34" ), (int)sfap::net::address_type::HOSTNAME );
    EXPECT_EQ( (int)sfap::net::detect_address_type( "127.33.aaa.34" ), (int)sfap::net::address_type::HOSTNAME );

}

