/*!
 *  \file
 *  \brief Source file containing IOSocket class definitions.
 * 
 *  `IOSocket` is wrapper around TCP socket.
 *  Supports SSL and makes easy data serialization.
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


#include <utility>
#include <atomic>
#include <mutex>
#include <optional>
#include <string>

#ifndef _WIN32

    #include <unistd.h>

#endif

#include <openssl/ssl.h>

#include <sfap.hpp>
#include <crypto/context/context.hpp>
#include <net/address/host.hpp>
#include <net/iosocket/iosocket.hpp>


using namespace sfap;
using namespace sfap::net;


IOSocket::IOSocket( socket_t fd, crypto::ssl_ptr ssl ) :
    _ssl( std::nullopt ),
    _sent_bytes( 0 ),
    _received_bytes( 0 ),
    _fd( fd )
{

    if ( _fd == INVALID_SOCKET ) {

        throw std::invalid_argument( "fd is invalid socket" );

    }

    if ( ssl ) {

        set_ssl( std::move( ssl ) );

    }

}


IOSocket::IOSocket( IOSocket&& other ) noexcept :
    _fd( std::exchange( other._fd, INVALID_SOCKET ) ),
    _ssl( std::move( other._ssl ) ),
    _sent_bytes( other._sent_bytes.load() ),
    _received_bytes( other._received_bytes.load() )
{

    other._sent_bytes.store( 0 );
    other._received_bytes.store( 0 );

}


IOSocket& IOSocket::operator=( IOSocket&& other ) noexcept {

    if ( this != &other ) {

        _fd = std::exchange( other._fd, INVALID_SOCKET );
        _ssl = std::move( other._ssl );

        _sent_bytes.store( other._sent_bytes.load() );
        other._sent_bytes.store( 0 );

        _received_bytes.store( other._received_bytes.load() );
        other._received_bytes.store( 0 );
    }

    return *this;

}


IOSocket::~IOSocket() {

    close();

}


void IOSocket::close() {

    if ( is_open() ) {

        if ( is_secure() ) {

            SSL* ssl = _ssl->get();
            const int result = SSL_shutdown( ssl );

            if ( result == 0 ) {

                SSL_shutdown( ssl );

            }

            _ssl.reset();

        }

        #ifdef _WIN32

            shutdown( _fd, SD_BOTH  );

        #else

            shutdown( _fd, SHUT_RDWR );

        #endif

        closesocket( _fd );
        
        _fd = INVALID_SOCKET;

    }

}


bool IOSocket::is_open() const noexcept {

    return _fd != INVALID_SOCKET;

}


IOSocket::operator bool() const noexcept {

    return is_open();

}


bool IOSocket::is_secure() const noexcept {

    return _ssl.has_value();

}


qword_t IOSocket::get_sent_bytes_count() noexcept {

    return _sent_bytes;

}


qword_t IOSocket::get_received_bytes_count() noexcept {

    return _received_bytes;

}


socket_t IOSocket::get_socket() const noexcept {

    return _fd;

}


Host IOSocket::get_local_address() const {

    sockaddr_storage addr{};
    socklen_t addr_len = sizeof( addr );

    if ( !is_open() ) {

        throw std::logic_error( "socket is closed" );

    }

    if ( getsockname( _fd, reinterpret_cast<sockaddr*>( &addr ), &addr_len ) < 0 ) {

        throw std::system_error( errno, std::generic_category(), "can't get local address (getsockname)" );

    }

    return Host( addr );

}


Host IOSocket::get_remote_address() const {

    sockaddr_storage addr{};
    socklen_t addr_len = sizeof( addr );

    if ( !is_open() ) {

        throw std::logic_error( "socket is closed" );

    }

    if ( getpeername( _fd, reinterpret_cast<sockaddr*>( &addr ), &addr_len ) < 0 ) {

        throw std::system_error( errno, std::generic_category(), "can't get remote address (getpeername)" );

    }

    return Host( addr );

}


void IOSocket::set_ssl( crypto::ssl_ptr ssl ) {

    if ( _ssl ) _ssl.reset();

    _ssl = std::move( ssl );

}