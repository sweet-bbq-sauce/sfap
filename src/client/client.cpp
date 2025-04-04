#include <client/client.hpp>


using namespace sfap;
using namespace sfap::protocol;


Client::Client( const net::Address& address, std::optional<Credentials> credentials, std::optional<path_t> path ) :
    _sock( net::connect( address ) ),
    _address( address ) {

    if ( credentials ) login( credentials.value() );

    if ( credentials && path ) cd( path.value() );

}


Client::Client( const Client& other ) : Client( other._address, other._credentials ) {

    cd( other.pwd() );

}


Client::Client( Client&& other ) :
    _sock( std::move( other._sock ) ),
    _credentials( std::move( other._credentials ) ),
    _address( other._address ) {}


Client& Client::operator=( Client&& other ) {

    if ( &other != this ) {

        _sock = std::move( other._sock );
        _credentials = std::move( other._credentials );
        _address = other._address;

    }

    return *this;

}


Client::~Client() {

    close();

}


Client Client::clone() const {

    Client temp( _address, _credentials );

    temp.cd( pwd() );

    return temp;

}