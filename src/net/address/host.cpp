/*!
 *  \file
 *  \brief Source file containing Host class definition.
 * 
 *  `Host` stores `hostname:port` pair.
 *
 *  \copyright Copyright (c) 2025 Wiktor Sołtys
 *
 *  \cond
 *  MIT License
 * 
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  \endcond
 */


#include <charconv>
#include <stdexcept>
#include <string>

#ifdef _WIN32

    #include <winsock2.h>
    #include <ws2tcpip.h>

#else

    #include <arpa/inet.h>
    #include <netinet/in.h>
    
#endif

#include <sfap.hpp>
#include <net/address/detect.hpp>
#include <net/address/host.hpp>
#include <net/resolver/resolver.hpp>


using namespace sfap;
using namespace sfap::net;


Host::Host() noexcept :
    _port( 0 ),
    _type( address_type::EMPTY )
{}


Host::Host( const std::string& hostname, port_t port ) {

    set_hostname( hostname );
    set_port( port );

}


Host::Host( const std::string& host ) {

    set_host( host );

}


Host::Host( const sockaddr_storage& native ) {

    from_native( native );

}


void Host::from_native( const sockaddr_storage& native ) {

    const void* address;
    port_t port;

    if ( native.ss_family == AF_INET ) {

        const auto ipv4 = reinterpret_cast<const sockaddr_in*>( &native );

        port = ntohs( ipv4->sin_port );
        address = &ipv4->sin_addr;

    }
    else if ( native.ss_family == AF_INET6 ) {

        const auto ipv6 = reinterpret_cast<const sockaddr_in6*>( &native );

        port = ntohs( ipv6->sin6_port );
        address = &ipv6->sin6_addr;

    }
    else {

        throw std::invalid_argument( "unsupported address family" );

    }

    char buffer[INET6_ADDRSTRLEN];

    if ( inet_ntop( native.ss_family, address, buffer, sizeof( buffer ) ) == nullptr ) {

        throw std::runtime_error( "can't translate address from network to string (inet_ntop)" );

    }

    set_hostname( buffer );
    set_port( port );

}


bool Host::operator==( const Host& other ) const noexcept {

    return _hostname == other._hostname && _port == other._port;

}


bool Host::operator==( const std::string& hostname ) const noexcept {

    return _hostname == hostname;

}


void Host::set_hostname( const std::string& hostname ) {

    const auto type = detect_address_type( hostname );

    if ( type == address_type::EMPTY ) {

        throw std::invalid_argument( "address is empty" );

    }
    
    if ( type == address_type::UNSUPPORTED ) {

        throw std::invalid_argument( "unsupported address" );

    }

    if ( type == address_type::HOSTNAME ) {

        _hostname = hostname;
        _type = type;

        return;
        
    }

    if ( type == address_type::IPV4 || type == address_type::IPV6 ) {

        char buffer[INET6_ADDRSTRLEN];
        char address[sizeof( in6_addr )];
        const int family = ( type == address_type::IPV4 ) ? AF_INET : AF_INET6;

        if ( inet_pton( family, hostname.c_str(), address ) != 1 ) {

            throw std::runtime_error( "can't parse '" + hostname + "' as " + ( ( family == AF_INET ) ? "IPv4" : "IPv6" ) + " (inet_pton)" );

        }

        if ( inet_ntop( family, address, buffer, sizeof( buffer ) ) == nullptr ) {

            throw std::runtime_error( "can't stringify address (inet_ntop)" );

        }

        _hostname = buffer;
        _type = type;

        return;

    }

    throw std::runtime_error( "unreachable: unknown address type" );

}


std::string Host::get_hostname() const noexcept {

    return _hostname;

}


void Host::set_port( port_t port ) noexcept {

    _port = port;

}


port_t Host::get_port() const noexcept {

    return _port;

}


void Host::set_host( const std::string& host ) {

    if ( host.empty() ) {

        throw std::invalid_argument( "address is empty" );

    }

    std::string hostname, port_string;

    if ( host.front() == '[' ) {

        const auto closing_bracket = host.find( ']' );

        if ( closing_bracket == std::string::npos ) {

            throw std::invalid_argument( "missing ']' in the address" );

        }

        if ( closing_bracket + 1 >= host.size() || host[closing_bracket + 1] != ':' ) {

            throw std::invalid_argument( "missing ':' after ']' in the address" );

        }

        hostname = host.substr( 1, closing_bracket - 1 );
        port_string = host.substr( closing_bracket + 2 );

    }
    else {

        const auto doubledot = host.find( ':' );
        
        if ( doubledot == std::string::npos ) {

            throw std::invalid_argument( "missing ':' in the address" );

        }

        hostname = host.substr( 0, doubledot );
        port_string = host.substr( doubledot + 1 );

    }

    if ( hostname.empty() ) {

        throw std::invalid_argument( "hostname is empty" );

    }

    if ( port_string.empty() ) {

        throw std::invalid_argument( "port is empty" );

    }

    long port;
    const auto [ptr, ec] = std::from_chars( port_string.data(), port_string.data() + port_string.size(), port );

    if ( ec != std::errc() || ptr != port_string.data() + port_string.size() ) {

        throw std::invalid_argument( "invalid port value" );

    }

    if ( port < 0 || port > std::numeric_limits<port_t>::max() ) {

        throw std::invalid_argument( "port value is out of port range (0-65535)" );

    }

    set_hostname( hostname );
    set_port( static_cast<port_t>( port ) );

}


address_type Host::get_address_type() const noexcept {

    return _type;

}


std::string Host::to_string() const noexcept {

    if ( _type == address_type::IPV6 ) {

        return "[" + _hostname + "]:" + std::to_string( _port );

    }

    return _hostname + ":" + std::to_string( _port );

}


bool Host::is_valid() const noexcept {

    return !_hostname.empty() && ( _type != address_type::EMPTY && _type != address_type::UNSUPPORTED );

}


Host::operator bool() const noexcept {

    return is_valid();

}


bool Host::is_connectable() const noexcept {

    return is_valid() && _port != 0;

}


sockaddr_storage Host::to_native() const {

    const std::string canonical = Resolver( _hostname ).get_auto();
    const address_type type = detect_address_type( canonical );

    sockaddr_storage buffer{};
    const int family = ( type == address_type::IPV4 ) ? AF_INET : AF_INET6;
    void* address;

    buffer.ss_family = family;

    if ( family == AF_INET ) {

        auto ipv4 = reinterpret_cast<sockaddr_in*>( &buffer );

        ipv4->sin_port = htons( _port );
        address = &ipv4->sin_addr;

    }
    else {

        auto ipv6 = reinterpret_cast<sockaddr_in6*>( &buffer );

        ipv6->sin6_port = htons( _port );
        address = &ipv6->sin6_addr;

    }

    const int result = inet_pton( family, canonical.c_str(), address );

    if ( result <= 0 ) {

        throw std::runtime_error( "can't translate from string to network address (inet_pton)" );

    }

    return buffer;

}