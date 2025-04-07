#include <client/client.hpp>


using namespace sfap;
using namespace sfap::protocol;


std::string Client::whoami() const {

    _negotiate_command( CommandList::WHOAMI );

    return _sock.recvss();

}


path_t Client::home() const {

    _negotiate_command( CommandList::HOME );

    return _sock.recvss();

}


LoginResult Client::login( const Credentials& credentials ) {

    _negotiate_command( CommandList::LOGIN );

    _sock.sends( credentials.user );
    _sock.sends( credentials.password );

    const auto result = _sock.recvo<LoginResult>();

    if ( result == LoginResult::OK ) _credentials = credentials;

    return result;

}


void Client::logout() {

    _negotiate_command( CommandList::LOGOUT );

    _credentials.reset();

}


bool Client::is_logged() const {

    _negotiate_command( CommandList::IS_LOGGED);

    return _sock.recvo<bool>();

}