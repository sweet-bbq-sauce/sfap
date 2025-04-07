#include <string>
#include <stdexcept>

#include <openssl/ssl.h>

#include <sfap.hpp>
#include <net/address/host.hpp>
#include <crypto/context/context.hpp>
#include <net/iosocket/iosocket.hpp>
#include <crypto/utils.hpp>
#include <net/utils.hpp>


using namespace sfap::net;


void IOSocket::recv( void* data, qword_t size ) const {

    if ( size == 0 ) return;
    if ( data == nullptr ) throw std::invalid_argument( "data is null" );
    if ( !*this ) throw std::logic_error( "socket is closed" );

    qword_t received = 0;
    constexpr qword_t int_max = static_cast<qword_t>( INT_MAX );

    while ( received < size ) {

        const int to_receive = static_cast<int>( std::min( size - received, int_max ) );

        int n;

        if ( _ssl ) {

            n = SSL_read( _ssl->get(), static_cast<char*>( data ) + received, to_receive );

            if ( n < 0 ) throw crypto::SSLException( SSL_get_error( _ssl->get(), n ) );
            else if ( n == 0 ) throw std::runtime_error( "SSL_read failed: remote peer closed" );

        }

        else {

            n = ::recv( _fd, static_cast<char*>( data ) + received, to_receive, 0 );

            if ( n < 0 ) throw SocketException();
            else if ( n == 0 ) throw std::runtime_error( "recv failed: remote peer closed" );

        }

        received += static_cast<qword_t>( n );
        _received += static_cast<qword_t>( n );

    }

}


void IOSocket::recvd( data_t& data ) const {

    const qword_t size = recvo<qword_t>();

    if ( size == 0 ) {

        data.clear();

        return;

    }

    data = data_t( size );

    recv( data.data(), size );

}


void IOSocket::recvs( std::string& data ) const {

    const qword_t size = recvo<qword_t>();

    if ( size == 0 ) {

        data.clear();
        
        return;

    }

    data.resize( size );

    recv( data.data(), size );

    data.shrink_to_fit();

}


std::string IOSocket::recvss() const {

    std::string buffer;

    recvs( buffer );

    return buffer;

}


void IOSocket::recvsv( std::vector<std::string>& data ) const {

    const auto size = recvo<qword_t>();

    data = std::vector<std::string>( size );

    for ( qword_t i = 0; i < size; i++ ) data.push_back( recvss() );

}