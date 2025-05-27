/*!
 *  \file
 *  \brief Source file containing Socks5 connect procedure.
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
#include <string>
#include <string_view>

#include <net/address/host.hpp>
#include <net/iosocket/iosocket.hpp>
#include <net/proxy/proxy.hpp>
#include <utils/credentials.hpp>


using namespace sfap;
using namespace sfap::net;


/*!
 *  \brief SOCKS5h CONNECT protocol handshake (with optional authentication).
 *
 *  \details
 *  This outlines the SOCKS5 (SOCKS version 5) CONNECT handshake sequence,
 *  commonly used with proxies, including hostname resolution via the proxy (`socks5h`).
 *
 *  The protocol steps are:
 *
 *  1. **Client greeting** (methods supported):
 *      \code
 *      +----+----------+----------+
 *      |VER | NMETHODS | METHODS  |
 *      +----+----------+----------+
 *      | 1  |    1     | 1 to 255 |
 *      +----+----------+----------+
 *      \endcode
 *       - VER: SOCKS version (0x05)
 *       - NMETHODS: number of methods
 *       - METHODS: supported auth methods
 *          - 0x00: No authentication
 *          - 0x02: Username/Password
 *
 *  2. **Server selects method**:
 *      \code
 *      +----+--------+
 *      |VER | METHOD |
 *      +----+--------+
 *      | 1  |   1    |
 *      +----+--------+
 *      \endcode
 *
 *  3. **(If 0x02) Username/Password authentication**:
 *      \code
 *      +----+------+----------+------+----------+
 *      |VER | ULEN |  UNAME   | PLEN |  PASSWD  |
 *      +----+------+----------+------+----------+
 *      | 1  |  1   | 1 to 255 |  1   | 1 to 255 |
 *      +----+------+----------+------+----------+
 *      \endcode
 *      Server replies:
 *      \code
 *      +----+--------+
 *      |VER | STATUS |
 *      +----+--------+
 *      | 1  |   1    |
 *      +----+--------+
 *      STATUS:
 *       - 0x00 = success
 *       - any other = failure
 *      \endcode
 *
 *  4. **Client sends CONNECT request**:
 *      \code
 *      +----+-----+-------+------+----------+----------+
 *      |VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
 *      +----+-----+-------+------+----------+----------+
 *      | 1  |  1  | X'00' |  1   | Variable |    2     |
 *      +----+-----+-------+------+----------+----------+
 *      \endcode
 *       - CMD: 0x01 = CONNECT
 *       - ATYP: address type
 *          - 0x01 = IPv4 (4 bytes)
 *          - 0x03 = Domain name (1 byte length + domain)
 *          - 0x04 = IPv6 (16 bytes)
 *
 *  5. **Server reply**:
 *      \code
 *      +----+-----+-------+------+----------+----------+
 *      |VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
 *      +----+-----+-------+------+----------+----------+
 *      | 1  |  1  | X'00' |  1   | Variable |    2     |
 *      +----+-----+-------+------+----------+----------+
 *      \endcode
 *       - REP: reply field, 0x00 = succeeded
 *       - other values indicate errors (e.g. 0x01 = general failure)
 *
 *  \note
 *  For `socks5h`, the domain name is **not resolved locally**; the proxy performs DNS resolution.
 *
 *  \see RFC 1928 (SOCKS Protocol Version 5)
 *  \see RFC 1929 (Username/Password Authentication for SOCKS V5)
 */


const auto expected_byte = []( byte_t value, byte_t expected_value ) -> void {

    if ( value != expected_value ) {
        
        throw std::runtime_error( "unexpected behavior from the socks5 server. expected: " + std::to_string( (int)expected_value ) + " got: " + std::to_string( (int)value ) );

    }

};


void Proxy::_authenticate_socks5( const IOSocket& sock ) const {

    std::vector<byte_t> methods;

    methods.push_back( 0x00 );
    if ( _credentials ) methods.push_back( 0x02 );

    if ( methods.size() > 255 ) {

        throw std::logic_error( "too many methods given" );

    }

    sock.sendc( 0x05 );
    sock.sendo( static_cast<byte_t>( methods.size() ) );
    sock.send( methods.data(), static_cast<dword_t>( methods.size() ) );

    expected_byte( sock.recvc(), 0x05 );

    const byte_t result_method = sock.recvc();

    if ( result_method == 0xFF ) {

        throw std::runtime_error( "socks5 server refuses all authentication methods" );

    }
    else if ( result_method == 0x00 ) {

        return;

    }
    else if ( result_method == 0x02 ) {

        if ( !_credentials ) {

            throw std::runtime_error( "socks5 server wants user:pass authentication method" );

        }

        const auto& credentials = _credentials.value().get();
        const std::string_view user = credentials.get_user();
        const std::string_view password = credentials.get_password();

        if ( user.size() > 255 ) {

            throw std::invalid_argument( "user is too long (>255)" );

        }

        if ( password.size() > 255 ) {

            throw std::invalid_argument( "password is too long (>255)" );

        }

        sock.sendc( 0x01 );

        sock.sendo( static_cast<byte_t>( user.size() ) );
        sock.send( user.data(), static_cast<dword_t>( user.size() ) );

        sock.sendo( static_cast<byte_t>( password.size() ) );
        sock.send( password.data(), static_cast<dword_t>( password.size() ) );

        expected_byte( sock.recvc(), 0x01 );

        if ( sock.recvc() != 0x00 ) {

            throw std::runtime_error( "socks5 server refuses authentication" );

        }

        // OK

    }
    else {

        throw std::runtime_error( "socks5 server wants unsupported authentication method" );

    }

}


void Proxy::_connect_socks5( const Host& target, const IOSocket& sock ) const {

    if ( !target.is_connectable() ) {

        throw std::invalid_argument( "target address is not connectable" );

    }

    sock.sendc( 0x05 );     // Socks version
    sock.sendc( 0x01 );     // CONNECT command
    sock.sendc( 0x00 );     // Reserved

    if ( target.get_type() == address_type::HOSTNAME ) {

        const std::string_view hostname( target.get_hostname() );

        if ( hostname.size() > 255 ) {

            throw std::runtime_error( "hostname is too long (>255)" );

        }

        sock.sendc( 0x03 );

        sock.sendo( static_cast<byte_t>( hostname.size() ) );
        sock.send( hostname.data(), static_cast<dword_t>( hostname.size() ) );
        sock.sendo( target.get_port() );

    }
    else if ( target.get_type() == address_type::IPV4 ) {

        const auto native = target.to_native();
        const auto ipv4 = reinterpret_cast<const sockaddr_in*>( &native );

        sock.sendc( 0x01 );

        sock.send( &ipv4->sin_addr, static_cast<dword_t>( sizeof( ipv4->sin_addr ) ) );
        sock.send( &ipv4->sin_port, static_cast<dword_t>( sizeof( ipv4->sin_port ) ) );

    }
    else if ( target.get_type() == address_type::IPV6 ) {

        const auto native = target.to_native();
        const auto ipv6 = reinterpret_cast<const sockaddr_in6*>( &native );

        sock.sendc( 0x04 );

        sock.send( &ipv6->sin6_addr, static_cast<dword_t>( sizeof( ipv6->sin6_addr ) ) );
        sock.send( &ipv6->sin6_port, static_cast<dword_t>( sizeof( ipv6->sin6_port ) ) );

    }
    else {

        throw std::logic_error( "unreachable" );
        
    }

    expected_byte( sock.recvc(), 0x05 );

    if ( sock.recvc() != 0x00 ) {

        throw std::runtime_error( "socks5 server can't create tunnel to " + target.to_string() );

    }

    if ( sock.recvc() != 0x00 ) {

        throw std::runtime_error( "unexpected behavior from the socks5 server" );

    }

    const auto result_address_type = sock.recvc();
    char buffer[sizeof( in6_addr ) + 2];

    if ( result_address_type == 0x01 ) {

        sock.recv( buffer, 6 );

    }
    else if ( result_address_type == 0x04 ) {

        sock.recv( buffer, sizeof( buffer ) );

    }
    else if ( result_address_type == 0x03 ) {

        const auto hostname_size = sock.recvc();

        std::unique_ptr<char> domain( new char[ hostname_size ] );

        sock.recv( domain.get(), hostname_size );

    }
    else {

        throw std::runtime_error( "unexpected behavior from the socks5 server" );

    }
    
}


void Proxy::_open_socks5( const Host& target, const IOSocket& sock ) const {

    _authenticate_socks5( sock );
    _connect_socks5( target, sock );

}