/*!
 *  \file
 *  \brief Source file containing Listener class definitions.
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


#include <optional>
#include <system_error>

#ifndef _WIN32

    #include <unistd.h>

#endif

#include <crypto/exception.hpp>
#include <net/address/address.hpp>
#include <net/iosocket/iosocket.hpp>
#include <net/listener/listener.hpp>


using namespace sfap;
using namespace sfap::net;


Listener::Listener( const Address& address ) :
    _ssl_context( address.get_ssl_context() )
{

    if ( !address.get_target().is_valid() ) {

        throw std::runtime_error( "bind address is not valid" );

    }

    const auto native_address = address.get_target().to_native();
    
    _fd = socket( native_address.ss_family, SOCK_STREAM, IPPROTO_TCP );
    if ( _fd == INVALID_SOCKET ) {

        throw std::system_error( errno, std::generic_category(), "(::socket)" );

    }

    if ( bind( _fd, reinterpret_cast<const sockaddr*>( &native_address ), sizeof( native_address ) ) != 0 ) {

        throw std::system_error( errno, std::generic_category(), "(::bind)" );

    }

    if ( listen( _fd, SOMAXCONN ) != 0 ) {

        throw std::system_error( errno, std::generic_category(), "(::listen)" );

    }

}


Listener::~Listener() noexcept {

    close();

}


void Listener::close() noexcept {

    if ( is_open() ) {

        closesocket( _fd );

        _fd = INVALID_SOCKET;

    }

}


bool Listener::is_open() const noexcept {

    return _fd != INVALID_SOCKET;

}


Listener::operator bool() const noexcept {

    return is_open();

}


bool Listener::is_secure() const noexcept {

    return _ssl_context.has_value();

}


IOSocket Listener::accept( std::optional<std::reference_wrapper<Host>> peer_address ) const {

    if ( !is_open() ) {

        throw std::logic_error( "listener is closed" );

    }

    sockaddr_storage client_address{};
    socklen_t client_address_length = sizeof( client_address );

    IOSocket client( ::accept( _fd, reinterpret_cast<sockaddr*>( &client_address ), &client_address_length ) );
    if ( !client ) {

        throw std::system_error( errno, std::generic_category(), "(::accept)" );

    }

    if ( peer_address ) {

        peer_address.value().get().from_native( client_address );

    }

    if ( is_secure() ) {

        const auto& ssl_context = _ssl_context.value().get();
        auto ssl = ssl_context.create_ssl();

        if ( SSL_set_fd( ssl.get(), client.get_socket() ) != 1 ) {

            throw crypto::OpenSSLError();

        }

        if ( SSL_accept( ssl.get() ) != 1 ) {

            throw crypto::OpenSSLError();

        }

        client.set_ssl( std::move( ssl ) );

    }

    return client;

}


Host Listener::get_host() const {

    if ( !is_open() ) {

        throw std::logic_error( "listener is closed" );

    }

    // IOSocket has `get_local_host()` but `return IOSocket( _fd ).get_local_host();` will execute destructor and close socket.

    sockaddr_storage address{};
    socklen_t length = sizeof( address );
    if ( getsockname( _fd, reinterpret_cast<sockaddr*>( &address ), &length ) != 0 ) {

        throw std::system_error( errno, std::generic_category(), "(::getsockname)" );

    }

    return Host( address );

}