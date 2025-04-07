#include <client/client.hpp>
#include <utils/log.hpp>


using namespace sfap;
using namespace sfap::protocol;


Client::Client( const net::Address& address, std::optional<Credentials> credentials, std::optional<path_t> path ) :
    _sock( net::connect( address ) ),
    _address( address ) {

    _sock.sends( config::version );

    if ( _sock.recvo<ConnectionResult>() != ConnectionResult::OK ) {

        closesocket( _sock.get_socket() );

        throw std::runtime_error( "Connection rejected by server" );

    }

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


void Client::close() {

    _negotiate_command( CommandList::BYE );

    utils::info( "Received: ", _sock.get_received_bytes(), " Bytes" );
    utils::info( "Sent: ", _sock.get_sent_bytes(), " Bytes" );

}


ServerInfo Client::info() const {

    _negotiate_command( CommandList::INFO );

    ServerInfo info;

    _sock.recvs( info.version );
    _sock.recvsv( info.extensions );
    info.time = _sock.recvo<timestamp_t>();

    return info;

}


void Client::none() const {

    _negotiate_command( CommandList::NONE );

}


void Client::_negotiate_command( CommandList cmd ) const {

    _sock.sendo( precommand_header );     // Watchdog
    _sock.sendo( cmd );     // Command code

    const auto result = _sock.recvo<CommandResult>();

    if ( result != CommandResult::OK ) throw std::runtime_error( "Server denied command: " + std::to_string( static_cast<int>( result ) ) );

}