#include <string>

#include <net/address/host.hpp>
#include <net/address/address.hpp>
#include <net/dns/dns.hpp>


using namespace sfap::net;


DNS::DNS( const std::string& hostname, Mode mode ) :
    _mode( mode ),
    _has_ipv4( false ),
    _has_ipv6( false ) {

    _resolve( hostname );

}


DNS::DNS( const Host& host ) :
    DNS( host.get_hostname() ) {}


DNS::DNS( const Address& address ) :
    DNS( address.get_target() ) {}


void DNS::set_mode( Mode mode ) {

    _mode = mode;

}


bool DNS::has_ipv4() const noexcept {

    return _has_ipv4;

}


bool DNS::has_ipv6() const noexcept {

    return _has_ipv6;

}