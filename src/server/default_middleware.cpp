#include <vector>
#include <algorithm>
#include <iostream>

#include <sfap.hpp>
#include <protocol/protocol.hpp>


using namespace sfap;
using namespace sfap::protocol;


CommandResult default_command_middleware( CommandList command, const std::string& user, const bool& logged ) {

    return CommandResult::OK;

}


LoginResult default_login_middleware( const Credentials& credentials, std::string& username, bool& logged, path_t& root_directory, path_t& home_directory ) {

    return LoginResult::OK;

}