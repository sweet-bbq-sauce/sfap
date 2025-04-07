#include <memory>
#include <utility>
#include <stdexcept>
#include <optional>

#include <net/iosocket/iosocket.hpp>
#include <crypto/context/context.hpp>
#include <net/utils.hpp>
#include <utils/log.hpp>


using namespace sfap::net;


IOSocket::IOSocket( socket_t fd, std::optional<crypto::SSL_t> ssl ) :
    _fd( fd ),
    _ssl( std::move( ssl ) ),
    _sent( 0 ),
    _received( 0 )
    {}


void IOSocket::close() noexcept {

    if ( _fd != INVALID_SOCKET ) {

        if ( _ssl ) SSL_shutdown( _ssl->get() );

        closesocket( _fd );

        _fd = INVALID_SOCKET;

    }

}


IOSocket::~IOSocket() noexcept {

    close();

}


IOSocket::IOSocket( IOSocket&& other ) noexcept :

    _fd( std::exchange( other._fd, INVALID_SOCKET ) ),
    _ssl( std::move( other._ssl ) ),
    _sent( std::exchange( other._sent, 0 ) ),
    _received( std::exchange( other._received, 0 ) )
    {}


IOSocket& IOSocket::operator=( IOSocket&& other ) noexcept {

    if ( this != &other ) {

        _fd = std::exchange( other._fd, INVALID_SOCKET );
        _ssl = std::move( other._ssl );
        _sent = std::exchange( other._sent, 0 );
        _received = std::exchange( other._received, 0 );

    }

    return *this;

}


bool IOSocket::is_open() const noexcept {

    return ( _fd != INVALID_SOCKET );

}


bool IOSocket::is_secure() const noexcept {

    return ( _ssl != nullptr );

}


sfap::qword_t IOSocket::get_sent_bytes() const noexcept {

    return _sent;

}


sfap::qword_t IOSocket::get_received_bytes() const noexcept {

    return _received;

}


IOSocket::operator bool() const noexcept {

    return is_open();

}


sfap::socket_t IOSocket::get_socket() const noexcept {

    return _fd;

}


Host IOSocket::get_local_host() const {

    if ( !*this ) throw std::logic_error( "socket is closed" );

    sockaddr_storage temp = { 0 };
    socklen_t len = sizeof( temp );

    if ( getsockname( _fd, reinterpret_cast<sockaddr*>( &temp ), &len ) != 0 ) throw SocketException();

    return temp;

}


Host IOSocket::get_remote_host() const {

    if ( !*this ) throw std::logic_error( "socket is closed" );

    sockaddr_storage temp = { 0 };
    socklen_t len = sizeof( temp );

    if ( getpeername( _fd, reinterpret_cast<sockaddr*>( &temp ), &len ) != 0 ) throw SocketException();

    return temp;

}