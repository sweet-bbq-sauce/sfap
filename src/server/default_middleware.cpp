#include <vector>
#include <algorithm>
#include <iostream>

#include <sfap.hpp>
#include <protocol/protocol.hpp>
#include <utils/log.hpp>
#include <server/command/command.hpp>


using namespace sfap;
using namespace sfap::protocol;


CommandResult default_command_middleware( CommandList command, const std::string& user, const bool& logged ) {

    utils::debug( "User \'", user, "\' requested \'", Commands::get_name( command ), "\'" );

    return CommandResult::OK;

}


LoginResult default_login_middleware( const Credentials& credentials, std::string& username, path_t& root_directory, path_t& home_directory ) {

    username = credentials.user;

    root_directory = std::getenv( "HOME" );
    home_directory = root_directory;

    utils::debug( "User logged as \'", credentials.user, "\' (", root_directory.string(), ")" );

    return LoginResult::OK;

}


ConnectionResult default_connection_middleware( const net::Host& host, const std::string& version ) {

    utils::debug( "New connection from ", host.get_hostname(), " (", version, ")" );

    return ConnectionResult::OK;

}