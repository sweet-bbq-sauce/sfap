/*!
 *  \file
 *  \brief Source file containing IOSocket receiving methods definitions.
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


#include <mutex>
#include <string>
#include <system_error>

#ifndef _WIN32

    #include <unistd.h>

#endif

#include <openssl/ssl.h>

#include <sfap.hpp>
#include <crypto/context/context.hpp>
#include <crypto/exception.hpp>
#include <net/address/host.hpp>
#include <net/iosocket/iosocket.hpp>
#include <utils/encoding.hpp>


using namespace sfap;
using namespace net;


void IOSocket::recv( void* data, dword_t size ) const {

    if ( !is_open() ) {

        throw std::logic_error( "socket is closed" );

    }

    if ( size == 0 ) return;

    if ( data == nullptr ) {

        throw std::invalid_argument( "data is null" );

    }

    dword_t received = 0;
    constexpr dword_t int_max = std::numeric_limits<dword_t>::max();

    std::unique_lock lock( _recv_mutex );

    while ( received < size ) {

        const int to_receive = static_cast<int>( std::min( size - received, int_max ) );
        int n;

        if ( is_secure() ) {

            n = SSL_read( _ssl->get(), static_cast<char*>( data ) + received, to_receive );

            if ( n <= 0 ) {

                const int error = SSL_get_error( _ssl->get(), n );

                if ( error == SSL_ERROR_ZERO_RETURN ) {

                    throw std::runtime_error( "remote peer disconnected (SSL_read)" );

                }
                else if ( error == SSL_ERROR_SYSCALL ) {

                    if ( errno == 0 ) {

                        throw std::runtime_error( "unknown error (SSL_read)" );

                    }
                    else {

                        throw std::system_error( errno, std::generic_category(), "(SSL_read)" );

                    }

                }

                throw crypto::OpenSSLError();

            }

        }
        else {

            n = ::recv( _fd, static_cast<char*>( data ) + received, to_receive, 0 );

            if ( n < 0 ) throw std::system_error( errno, std::generic_category(), "(::recv)" );
            else if ( n == 0 ) throw std::runtime_error( "remote peer disconnected (::recv)" );

        }

        received += static_cast<dword_t>( n );
        _received_bytes += static_cast<qword_t>( n );

    }
    
}


bool IOSocket::recvb() const {

    if ( recvo<byte_t>() > static_cast<byte_t>( 0 ) ) return true;
    else return false;

}


byte_t IOSocket::recvc() const {

    return recvo<byte_t>();

}


void IOSocket::recvd( data_t& data ) const {

    const dword_t size = recvo<dword_t>();

    if ( size == 0 ) {

        data.clear();

        return;

    }

    try {

        data.resize( size );

    }
    catch ( const std::bad_alloc& ) {

        throw std::runtime_error( "not enough memory" );

    }

    recv( data.data(), static_cast<dword_t>( data.size() ) );

}


void IOSocket::recvs( std::string& data ) const {

    const dword_t size = recvo<dword_t>();

    if ( size == 0 ) {

        data.clear();

        return;

    }

    try {

        data.resize( size );

    }
    catch ( const std::bad_alloc& ) {

        throw std::runtime_error( "not enough memory" );

    }

    recv( data.data(), static_cast<dword_t>( data.size() ) );

}


std::string IOSocket::recvss() const {

    std::string buffer;

    recvs( buffer );

    return buffer;

}


path_t IOSocket::recvp() const {

    return utils::string_to_path( recvss() );

}