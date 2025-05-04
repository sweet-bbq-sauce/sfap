#ifdef _WIN32

    #include <winsock2.h>

#endif

#include <gtest/gtest.h>

#include <net/resolver/resolver.hpp>


using namespace sfap;
using namespace sfap::net;


// Test poprawnego rozwiązania hosta
TEST( Resolver, ResolveValidHostname ) {

    #ifdef _WIN32

        WSADATA wsaData;
        if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0) FAIL() << "WSAStartup failed";

    #endif

    Resolver resolver( "example.com", Resolver::Mode::IPV4 );
    EXPECT_TRUE( resolver.has_ipv4() );
    EXPECT_FALSE( resolver.get_all_ipv4().empty() );
    EXPECT_NO_THROW( resolver.get_random_ipv4() );

}

// Test rozpoznania IPv4 bez DNS
TEST( Resolver, DirectIPv4Address ) {
    Resolver resolver( "8.8.8.8" );
    EXPECT_TRUE( resolver.has_ipv4() );
    EXPECT_FALSE( resolver.has_ipv6() );
}

// Test rozpoznania IPv6 bez DNS
TEST( Resolver, DirectIPv6Address ) {
    Resolver resolver( "2001:4860:4860::8888" );
    EXPECT_TRUE( resolver.has_ipv6() );
    EXPECT_FALSE( resolver.has_ipv4() );
}

// Test błędnego hosta
TEST( Resolver, InvalidHostnameThrows ) {
    EXPECT_THROW({
        Resolver resolver( "this.host.does.not.exist" );
    }, ResolverError );
}

// Test pustego adresu
TEST( Resolver, EmptyHostnameThrows ) {
    EXPECT_THROW({
        Resolver resolver( "" );
    }, std::invalid_argument );
}

// Test domyślnego trybu
TEST( Resolver, DefaultResolveMode ) {
    Resolver resolver( "example.com" );
    EXPECT_TRUE( resolver.has_ipv4() || resolver.has_ipv6() );
}

// Test klasy ResolverError
TEST( ResolverErrorTest, StoresErrorCode ) {
    ResolverError err( 4 );
    EXPECT_EQ( err.code(), 4 );
    EXPECT_FALSE( err.what() == nullptr );
}