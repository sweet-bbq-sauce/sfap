/*!
 *  \file
 *  \brief Source file containing default middlewares definitions.
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


#include <filesystem>
#include <iostream>
#include <mutex>
#include <optional>

#include <server/server.hpp>
#include <utils/credentials.hpp>


using namespace sfap;
using namespace sfap::protocol;
using namespace sfap::utils;


std::mutex _cout_mutex;


const Server::AuthMiddleware Server::default_authorize_middleware = []( const Credentials& credentials, std::string& username, path_t& root, std::optional<path_t> home ) -> AuthResult {

    username = credentials.get_user();
    root = std::filesystem::current_path();

    std::lock_guard lock( _cout_mutex );

    std::cout << "User '" << username << "' authorized with chroot: " << root << "." << std::endl;

    return AuthResult::OK;

};


const Server::CommandMiddleware Server::default_command_middleware = []( word_t id, const std::optional<std::string>& user ) -> CommandResult {

    std::lock_guard lock( _cout_mutex );
    
    std::cout << ( user ? ( "User '" + user.value() + "'" ) : "Anonymous user" ) << " requested command: " << id << std::endl;

    return ( user ) ? CommandResult::OK : CommandResult::ACCESS_DENIED;

};