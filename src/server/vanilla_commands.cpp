/*!
 *  \file
 *  \brief Source file containing `Server` class definition.
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


#include <algorithm>
#include <atomic>
#include <chrono>
#include <list>
#include <stdexcept>
#include <thread>

#include <net/address/address.hpp>
#include <net/listener/listener.hpp>
#include <server/command_registry/command_registry.hpp>
#include <server/server.hpp>
#include <server/session/session.hpp>
#include <utils/encoding.hpp>


using namespace sfap;
using namespace sfap::protocol;


const CommandRegistry protocol::vanilla_commands = ( []() -> CommandRegistry {


    CommandRegistry buffer;


    buffer.add( Command::NONE, "none", nullptr );
    buffer.add( Command::BYE, "bye", nullptr );
    buffer.add( Command::SERVER_INFO, "server_info", nullptr );
    buffer.add( Command::COMMANDS, "commands", nullptr );
    buffer.add( Command::DESCRIPTORS, "descriptors", nullptr );
    buffer.add( Command::AUTH, "auth", nullptr );
    buffer.add( Command::CLEAR, "clear", nullptr );


    buffer.add( Command::CD, "cd", []( Session& session, const net::IOSocket& socket ) {

        auto& filesystem = session.get_filesystem();

        const path_t path = socket.recvp();

        if ( std::filesystem::is_directory( filesystem.to_system( path ) ) ) {

            const auto result = filesystem.cd( path );

            socket.sende( result );

            if ( result == AccessResult::OK ) {

                socket.sendp( filesystem.pwd() );

            }

        }
        else {

            socket.sende( AccessResult::IS_NOT_DIRECTORY );

        }

    });


    buffer.add( Command::PWD, "pwd", []( Session& session, const net::IOSocket& socket ) {

        socket.sendp( session.get_filesystem().pwd() );

    });


    buffer.add( Command::HOME, "home", []( Session& session, const net::IOSocket& socket ) {

        socket.sendp( session.get_filesystem().get_home() );

    });


    return buffer;

    
})();