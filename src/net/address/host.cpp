#include <string>
#include <stdexcept>
#include <string_view>
#include <charconv>

#include <net/address/host.hpp>
#include <net/address/detect.hpp>
#include <net/dns/dns.hpp>
#include <sfap.hpp>
#include <net/utils.hpp>
#include <utils/misc.hpp>


using namespace sfap;
using namespace sfap::net;


Host::Host() noexcept :
    _port( 0 ) {}


Host::Host( const std::string& hostname, port_t port ) :
    _port( port ) {

    set_hostname( hostname );

}


Host::Host( const std::string& host ) {

    const auto separator = host.find( ':' );

    if ( separator == std::string_view::npos ) throw std::invalid_argument( "host must be in format \"hostname:port\"" );

    set_hostname( host.substr( 0, separator ) );

    std::string_view port_string = host.substr( separator + 1 );
    long int value = 0;

    const auto [ptr, ec] = std::from_chars( port_string.data(), port_string.data() + port_string.size(), value );

    if ( ec != std::errc() || ptr != port_string.data() + port_string.size() ) throw std::invalid_argument( "port is invalid" );

    if ( value < 0 || value > UINT16_MAX ) throw std::out_of_range( "port number must be in range 0-65535" );

    set_port( static_cast<port_t>( value ) );

}


Host::Host( const sockaddr_storage& native ) {

    const int family = native.ss_family;
    const void* address;

    if ( family == AF_INET ) {

        const auto ipv4 = reinterpret_cast<const sockaddr_in*>( &native );
                        
        set_port( ntohs( ipv4->sin_port ) );

        address = &ipv4->sin_addr;

    }
    else if ( family == AF_INET6 ) {

        const auto ipv6 = reinterpret_cast<const sockaddr_in6*>( &native );

        set_port( ntohs( ipv6->sin6_port ) );

        address = &ipv6->sin6_addr;

    }
    else throw std::invalid_argument( "address family not supported" );

    char buffer[INET6_ADDRSTRLEN];

    if ( inet_ntop( family, address, buffer, sizeof( buffer ) ) == nullptr ) throw SocketException();

    set_hostname( buffer );

}


bool Host::operator==( const Host& other ) const noexcept {

    return ( other._hostname == _hostname ) && ( other._port == _port );

}


bool Host::operator==( const std::string& hostname ) const noexcept {

    return _hostname == hostname;

}


Host& Host::operator=( const Host& other ) noexcept {

    _hostname = other._hostname;
    _port = other._port;

    return *this;

}


bool Host::is_valid() const noexcept {

    return !_hostname.empty();

}


bool Host::is_connectable() const noexcept {

    return !_hostname.empty() && ( _port != 0 );

}


std::string Host::string() const noexcept {

    return _hostname + ":" + std::to_string( _port );

}


void Host::set_hostname( const std::string& hostname ) {

    const auto type = detect_address_type( hostname );

    if ( type == AddressType::EMPTY ) throw std::invalid_argument( "hostname is empty" );
    else if ( type == AddressType::UNSUPPORTED ) throw std::invalid_argument( "unsupported address family" );
    else _hostname = hostname;

}


const std::string& Host::get_hostname() const noexcept {

    return _hostname;

}


void Host::set_port( port_t port ) noexcept {

    _port = port;

}


sfap::port_t Host::get_port() const noexcept {

    return _port;

}


sockaddr_storage Host::to_native() const {

    const std::string canonical = DNS( _hostname ).get_auto();
    const auto type = detect_address_type( canonical );
                    
    sockaddr_storage buffer = { 0 };
    const int family = ( type == AddressType::IPV4 ) ? AF_INET : AF_INET6;
    void* address;

    buffer.ss_family = family;

    if ( family == AF_INET ) {

        auto ipv4 = reinterpret_cast<sockaddr_in*>( &buffer );

        ipv4->sin_family = family;
        ipv4->sin_port = htons( _port );
        address = &ipv4->sin_addr;

    }
    else {

        auto ipv6 = reinterpret_cast<sockaddr_in6*>( &buffer );

        ipv6->sin6_family = family;
        ipv6->sin6_port = htons( _port );
        address = &ipv6->sin6_addr;

    }

    const int inet_result = inet_pton( family, canonical.c_str(), address );

    if ( inet_result < 0 ) throw SocketException();
    else if ( inet_result == 0 ) throw std::runtime_error( utils::glue( "address string \"", canonical, "\" is not valid (", ( family == AF_INET ) ? "IPv4" : "IPv6" , ")" ) );
    else return buffer;

}