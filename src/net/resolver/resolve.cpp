/*!
 *  \file
 *  \brief Implementation of address resolution logic for the Resolver class.
 *
 *  This file provides the platform-independent and platform-specific implementation
 *  of the internal _resolve() method used to convert a hostname into one or more IP addresses.
 *  It supports both literal IP parsing and DNS resolution using system libraries.
 *
 *  It is automatically invoked by the Resolver constructor.
 *
 *  \note Uses `getaddrinfo()` and `inet_ntop()` under POSIX and Windows systems.
 *
 *  \copyright Copyright (c) 2025 Wiktor Sołtys
 *
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
 */


#include <stdexcept>
#include <string>

#ifdef _WIN32

    #include <winsock2.h>
    #include <ws2tcpip.h>

#else

    #include <arpa/inet.h>
    #include <netdb.h>

#endif

#include <sfap.hpp>
#include <net/address/detect.hpp>
#include <net/resolver/resolver.hpp>


using namespace sfap;
using namespace sfap::net;


void Resolver::_resolve( const std::string& hostname ) {

    const auto type = detect_address_type( hostname );

    if ( type == address_type::EMPTY ) throw std::invalid_argument( "empty address" );
    else if ( type == address_type::UNSUPPORTED ) throw std::invalid_argument( "invalid address" );
    
    else if ( type == address_type::IPV4 || type == address_type::IPV6 ) {
        
        _result[hostname] = type;

        if ( type == address_type::IPV4 ) _has_ipv4 = true;
        else _has_ipv6 = true;

    }
    else {

        struct addrinfo hints = {};
        struct addrinfo* res = nullptr;

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        const int status = getaddrinfo( hostname.c_str(), nullptr, &hints, &res );

        if ( status != 0 ) throw ResolverError( status );

        for ( const addrinfo *p = res; p != nullptr; p = p->ai_next ) {

            const auto family = p->ai_family;
            const void* address;

            if ( family == AF_INET ) {

                const auto ipv4 = reinterpret_cast<const sockaddr_in*>( p->ai_addr );

                address = &ipv4->sin_addr;

                _has_ipv4 = true;

            }
            else if ( family == AF_INET6 ) {

                const auto ipv6 = reinterpret_cast<const sockaddr_in6*>( p->ai_addr );

                address = &ipv6->sin6_addr;

                _has_ipv6 = true;

            }
            else continue;

            char buffer[INET6_ADDRSTRLEN];

            if ( inet_ntop( family, address, buffer, sizeof( buffer ) ) == nullptr ) throw std::runtime_error( "inet_ntop error" );

            _result[buffer] = ( family == AF_INET ) ? address_type::IPV4 : address_type::IPV6;

        }

        if ( res ) freeaddrinfo( res );

        if ( _result.empty() ) throw std::runtime_error( "can't return any records" );

    }

}