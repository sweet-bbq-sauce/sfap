#include <gtest/gtest.h>

#include <net/address/address.hpp>
#include <net/address/host.hpp>
#include <net/proxy/proxy.hpp>
#include <crypto/context/context.hpp>


using namespace sfap;
using namespace sfap::net;


TEST( Address, ConstructorWithOnlyTarget ) {

    const Host host( "example.com:443" );
    const Address address( host );

    EXPECT_EQ( address.get_target().get_hostname(), "example.com" );
    EXPECT_FALSE( address.has_ssl_context() );
    EXPECT_FALSE( address.has_proxy() );

}


TEST( Address, ConstructorWithProxies ) {

    const Host host( "target.com:80" );
    const Host proxy_host( "proxy.com:8080" );
    const Proxy proxy( proxy_type::HTTP_CONNECT, proxy_host );

    const std::vector<Proxy> proxies = { proxy };
    Address address( host, nullptr, proxies );

    EXPECT_EQ(address.get_target().get_hostname(), "target.com");
    EXPECT_TRUE(address.has_proxy());
    EXPECT_EQ(address.get_proxies().size(), 1);

}


TEST( Address, ConstructorWithSSL ) {

    const Host host( "secure.com:443" );
    auto ctx = std::make_shared<const crypto::TLSContext>();
    const Address address( host, ctx );

    EXPECT_EQ( address.get_target().get_hostname(), "secure.com" );
    EXPECT_TRUE( address.has_ssl_context() );
    EXPECT_FALSE( address.has_proxy() );

}


TEST( Address, ConstructorWithDefaultSSL ) {

    const Host host( "secure.com:443" );
    const Address address( host, crypto::TLSContext::default_client_context );

    EXPECT_EQ( address.get_target().get_hostname(), "secure.com" );
    EXPECT_TRUE( address.has_ssl_context() );
    EXPECT_FALSE( address.has_proxy() );

}


TEST( Address, FullConstructor ) {

    const Host host( "secureproxy.com:443" );
    const Host proxy_host( "proxy.com:3128" );
    const Proxy proxy( proxy_type::SOCKS5, proxy_host );
    const std::vector<Proxy> proxies = { proxy };

    auto ctx = std::make_shared<crypto::TLSContext>();
    const Address address( host, ctx, proxies );

    EXPECT_EQ( address.get_target().get_hostname(), "secureproxy.com" );
    EXPECT_TRUE( address.has_ssl_context() );
    EXPECT_TRUE( address.has_proxy() );
    EXPECT_EQ( address.get_proxies().size(), 1 );

}


TEST( Address, AddProxy ) {

    const Host host( "basic.com:80" );
    Address address( host );

    const Host proxy_host( "proxy.net:1080" );
    const Proxy proxy( proxy_type::SOCKS5, proxy_host );

    address.add_proxy( proxy );
    EXPECT_TRUE( address.has_proxy() );
    EXPECT_EQ( address.get_proxies().size(), 1 );

}
