/*!
 *  \file
 *  \brief Source file containing connect function definition.
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


#include <system_error>

#ifndef _WIN32

    #include <arpa/inet.h>
    #include <unistd.h>

#endif

#include <openssl/ssl.h>

#include <crypto/exception.hpp>
#include <net/address/address.hpp>
#include <net/connect/connect.hpp>
#include <net/iosocket/iosocket.hpp>


using namespace sfap;
using namespace sfap::net;


IOSocket net::connect( const Address& address ) {

    if ( !address.get_target().is_connectable() ) {

        throw std::runtime_error( "target is not connectable" );

    }

    const auto entry_address = ( address.has_proxy() ? address.get_proxies().front().get_host() : address.get_target() ).to_native();
    IOSocket sock( socket( entry_address.ss_family, SOCK_STREAM, IPPROTO_TCP ) );

    if ( !sock ) {

        throw std::system_error( errno, std::generic_category(), "(::socket)" );

    }

    if ( ::connect( sock.get_socket(), reinterpret_cast<const sockaddr*>( &entry_address ), sizeof( entry_address ) ) != 0 ) {

        throw std::system_error( errno, std::generic_category(), "(::connect)" );

    }

    if ( address.has_proxy() ) {

        const auto& proxies = address.get_proxies();

        for ( auto it = proxies.begin(); it != proxies.end(); ++it ) {

            const auto& proxy = *it;

            if ( std::next( it ) != proxies.end() ) {

                proxy.open( ( *std::next( it ) ).get_host(), sock );

            }
            else {

                proxy.open( address.get_target(), sock );

            }

        }

    }

    if ( address.has_ssl_context() ) {

        const auto& ssl_context = address.get_ssl_context();
        auto ssl = ssl_context->create_ssl();

        if ( SSL_set_fd( ssl.get(), sock.get_socket() ) != 1 ) {

            throw crypto::OpenSSLError();

        }

        if ( SSL_set_tlsext_host_name( ssl.get(), address.get_target().get_hostname().c_str() ) != 1 ) {

            throw crypto::OpenSSLError();

        }

        if ( SSL_connect( ssl.get() ) != 1 ) {

            throw crypto::OpenSSLError();

        }

        sock.set_ssl( std::move( ssl ) );

    }

    return sock;

}