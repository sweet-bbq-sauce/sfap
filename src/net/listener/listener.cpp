#include <stdexcept>

#include <net/address/address.hpp>
#include <net/iosocket/iosocket.hpp>
#include <crypto/utils.hpp>
#include <net/utils.hpp>

#include <net/listener/listener.hpp>


using namespace sfap::net;


Listener::Listener( const Address& address ) :
    _address( address ),
    _fd( INVALID_SOCKET ) {

    if ( !address.get_target().is_valid() ) throw std::invalid_argument( "address target is not valid" );

    const auto addr = address.get_target().to_native();

    _fd = socket( addr.ss_family, SOCK_STREAM, IPPROTO_TCP );
    if ( _fd == INVALID_SOCKET ) throw SocketException();

    if ( bind( _fd, reinterpret_cast<const sockaddr*>( &addr ), sizeof( addr ) ) != 0 ) throw SocketException();
    if ( listen( _fd, SOMAXCONN ) != 0 ) throw SocketException();

}


void Listener::close() noexcept {

    if ( _fd != INVALID_SOCKET ) {

        closesocket( _fd );

        _fd = INVALID_SOCKET;

    }

}


bool Listener::is_opened() const noexcept {

    return ( _fd != INVALID_SOCKET );

}


Listener::operator bool() const noexcept {

    return is_opened();

}


IOSocket Listener::accept() const {

    if ( !is_opened() ) throw std::logic_error( "listener is closed" );    // This error should not appear
                    
    socket_t client_fd = ::accept( _fd, nullptr, nullptr );

    if ( client_fd == INVALID_SOCKET ) throw SocketException();

    if ( _address.get_ssl_context() ) {

        crypto::SSL_t ssl = _address.get_ssl_context().get()->create_ssl();

        SSL_set_mode( ssl.get(), SSL_MODE_AUTO_RETRY | SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER );

        if ( SSL_set_fd( ssl.get(), client_fd ) != 1 ) {

            closesocket( client_fd );
        
            throw crypto::SSLException();
        
        }

        SSL_set_accept_state( ssl.get() );
        
        if ( SSL_accept( ssl.get() ) != 1 ) {
        
            closesocket( client_fd );
        
            throw crypto::SSLException();
        
        }

        return IOSocket( client_fd, std::move( ssl ) );

    }

    else return IOSocket( client_fd );

}
