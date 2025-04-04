#include <string>
#include <map>
#include <stdexcept>

#ifndef _WIN32
    #include <netdb.h>
    #include <arpa/inet.h>
#endif

#include <net/address/host.hpp>
#include <net/address/address.hpp>
#include <net/address/detect.hpp>
#include <net/dns/dns.hpp>
#include <net/utils.hpp>
#include <net/dns/utils.hpp>


using namespace sfap::net;


void DNS::_resolve( const std::string& hostname ) {

    const auto type = detect_address_type( hostname );

    if ( type == AddressType::EMPTY ) throw std::invalid_argument( "hostname is empty" );
    else if ( type == AddressType::UNSUPPORTED ) throw std::invalid_argument( "address family is unsupported" );

    else if ( ( type == AddressType::IPV4 ) || ( type == AddressType::IPV6 ) ) _result[ hostname ] = type;

    else {  // if AddressType::HOSTNAME

        addrinfo hints = { 0 };
        addrinfo *result;

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        const int gai_result = getaddrinfo( hostname.c_str(), nullptr, &hints, &result );

        if ( gai_result != 0 ) throw DNSException( gai_result );

        for ( const addrinfo *p = result; p != nullptr; p = p->ai_next ) {

            const auto family = p->ai_family;
            const void* address;

            if ( family == AF_INET ) {

                const auto ipv4 = reinterpret_cast<const sockaddr_in*>( p->ai_addr );

                address = &ipv4->sin_addr;

            }
            else if ( family == AF_INET6 ) {

                const auto ipv6 = reinterpret_cast<const sockaddr_in6*>( p->ai_addr );

                address = &ipv6->sin6_addr;

            }
            else continue;

            char buffer[INET6_ADDRSTRLEN];

            if ( inet_ntop( family, address, buffer, sizeof( buffer ) ) == nullptr ) throw SocketException();

            const auto type = ( family == AF_INET ) ? AddressType::IPV4 : AddressType::IPV6;
            _result[buffer] = type;

            ( type == AddressType::IPV4 ) ? _has_ipv4 = true : _has_ipv6 = true;

        }

        if ( result ) freeaddrinfo( result );

        if ( _result.empty() ) throw std::runtime_error( "getaddrinfo returned no records" );

    }

}