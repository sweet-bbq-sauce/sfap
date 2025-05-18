#include <stdexcept>

#include <gtest/gtest.h>

#include <net/address/host.hpp>


using namespace sfap::net;


TEST( HostTest, ConstructFromHostnameAndPort ) {

    Host h( "example.com", 80 );

    EXPECT_EQ( h.get_hostname(), "example.com" );
    EXPECT_EQ( h.get_port(), 80 );
    EXPECT_TRUE( h );
    EXPECT_TRUE( h.is_valid() );
    EXPECT_TRUE( h.is_connectable() );

}


TEST( HostTest, ConstructFromIPv4String ) {

    Host h( "192.168.0.1:12345" );

    EXPECT_EQ( h.get_hostname(), "192.168.0.1" );
    EXPECT_EQ( h.get_port(), 12345 );
    EXPECT_EQ( h.get_address_type(), address_type::IPV4 );
    EXPECT_EQ( h.to_string(), "192.168.0.1:12345" );

}


TEST( HostTest, ConstructFromIPv6String1 ) {

    Host h( "[::1]:8080" );

    EXPECT_EQ( h.get_hostname(), "::1" );
    EXPECT_EQ( h.get_port(), 8080 );
    EXPECT_EQ( h.get_address_type(), address_type::IPV6 );
    EXPECT_EQ( h.to_string(), "[::1]:8080" );

}


TEST( HostTest, ConstructFromIPv6String2 ) {

    Host h( "[2001:0db8:85a3::8a2e:0370:7334]:420" );

    EXPECT_EQ( h.get_hostname(), "2001:db8:85a3::8a2e:370:7334" );
    EXPECT_EQ( h.get_port(), 420 );
    EXPECT_EQ( h.get_address_type(), address_type::IPV6 );
    EXPECT_EQ( h.to_string(), "[2001:db8:85a3::8a2e:370:7334]:420" );

}


TEST( HostTest, ConstructFromIPv6String3 ) {

    Host h( "[2001:0db8:85a3:0000:0000:8a2e:0370:7334]:420" );

    EXPECT_EQ( h.get_hostname(), "2001:db8:85a3::8a2e:370:7334" );
    EXPECT_EQ( h.get_port(), 420 );
    EXPECT_EQ( h.get_address_type(), address_type::IPV6 );
    EXPECT_EQ( h.to_string(), "[2001:db8:85a3::8a2e:370:7334]:420" );

}


TEST( HostTest, ConstructFromEmptyHostThrows ) {

    EXPECT_THROW( Host( "" ), std::invalid_argument );

}


TEST( HostTest, MissingColonThrows ) {

    EXPECT_THROW( Host( "localhost" ), std::invalid_argument );

}


TEST( HostTest, MissingClosingBracketThrows ) {

    EXPECT_THROW( Host( "[::1:8080" ), std::invalid_argument );

}


TEST( HostTest, MissingPortThrows ) {

    EXPECT_THROW( Host( "localhost:" ), std::invalid_argument );

}


TEST( HostTest, InvalidPortThrows ) {

    EXPECT_THROW( Host( "localhost:abc" ), std::invalid_argument );

}


TEST( HostTest, OutOfRangePortThrows ) {

    EXPECT_THROW( Host( "localhost:99999" ), std::invalid_argument );

}


TEST( HostTest, ComparisonOperators ) {

    Host h1( "example.com", 1234 );
    Host h2( "example.com", 1234 );
    Host h3( "example.com", 4321 );
    Host h4( "other.com", 1234 );

    EXPECT_TRUE( h1 == h2 );
    EXPECT_FALSE( h1 == h3 );
    EXPECT_FALSE( h1 == h4 );
    EXPECT_TRUE( h1 == "example.com" );
    EXPECT_FALSE( h1 == "other.com" );

}


TEST( HostTest, ToNativeAndFromNativeLoopbackIPv4 ) {

    Host original( "127.0.0.1", 420 );
    sockaddr_storage native = original.to_native();

    ASSERT_EQ( native.ss_family, AF_INET );

    Host converted( native );

    EXPECT_EQ( converted.get_hostname(), "127.0.0.1" );
    EXPECT_EQ( converted.get_port(), 420 );
    EXPECT_EQ( converted.get_address_type(), address_type::IPV4 );
    EXPECT_EQ( converted.to_string(), "127.0.0.1:420" );

    EXPECT_EQ( original, converted );
    
}