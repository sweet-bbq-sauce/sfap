#include <string>
#include <optional>

#include <net/proxy/proxy.hpp>
#include <net/address/host.hpp>


using namespace sfap::net;


Proxy::Proxy( Method method, const Host& proxy, std::optional<Credentials> credentials ) : _method( method ), _proxy( proxy ), _credentials( credentials ) {}


void Proxy::open_tunnel( const Host& target ) const {

    switch ( _method ) {

        case Method::SOCKS5H:
            open_socks5h_tunnel( target );
            break;

    }

    throw std::invalid_argument( "proxy method not supported" );

}