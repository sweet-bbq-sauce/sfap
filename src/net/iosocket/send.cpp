#include <string>

#include <openssl/ssl.h>

#include <sfap.hpp>
#include <net/address/host.hpp>
#include <crypto/context/context.hpp>
#include <net/iosocket/iosocket.hpp>
#include <crypto/utils.hpp>
#include <net/utils.hpp>


using namespace sfap::net;


void IOSocket::send( const void* data, qword_t size ) const {

    if ( size == 0 ) return;
    if ( data == nullptr ) throw std::invalid_argument( "data is null" );
    if ( !*this ) throw std::logic_error( "socket is closed" );

    qword_t sent = 0;
    constexpr qword_t int_max = static_cast<qword_t>( INT_MAX );

    while ( sent < size ) {

        const int to_send = static_cast<int>( std::min( size - sent, int_max ) );

        int n;

        if ( _ssl ) {

            n = SSL_write( _ssl->get(), static_cast<const char*>( data ) + sent, to_send );

            if ( n < 0 ) throw crypto::SSLException( SSL_get_error( _ssl->get(), n ) );
            else if ( n == 0 ) throw std::runtime_error( "SSL_write failed: remote peer closed" );

        }
        else {

            n = ::send( _fd, static_cast<const char*>( data ) + sent, to_send, 0 );

            if ( n < 0 ) throw SocketException();
            else if ( n == 0 ) throw std::runtime_error( "send failed: remote peer closed" );

        }

        sent += static_cast<qword_t>( n );
        _sent += static_cast<qword_t>( n );

    }

}


void IOSocket::sendb( const void* data, qword_t size ) const {

    sendo( size );

    send( data, size );

}


void IOSocket::sendd( const data_t& data ) const {

    sendb( data.data(), data.size() );

}


void IOSocket::sends( const std::string& data ) const {

    sendb( data.data(), data.size() );

}


void IOSocket::sendsv( const std::vector<std::string>& data ) const {

    sendo( static_cast<qword_t>( data.size() ) );

    for ( const auto& value : data ) sends( value );

}