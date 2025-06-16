/*!
 *  \file
 *  \brief Source file containing `Session` class definition.
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


#include <atomic>
#include <mutex>
#include <optional>
#include <set>
#include <shared_mutex>
#include <thread>

#include <net/iosocket/iosocket.hpp>
#include <protocol/protocol.hpp>
#include <server/server.hpp>
#include <server/session/session.hpp>
#include <server/virtual_filesystem/virtual_filesystem.hpp>


using namespace sfap;
using namespace sfap::protocol;


std::mutex _log_mutex;

void temp_log( const std::string& text ) {

    std::lock_guard lock( _log_mutex );

    std::cout << text << std::endl;

}


Session::Session( net::IOSocket& socket, Server& parent, session_id_t id ) :
    _parent( parent ),
    _finished( false ),
    _socket( std::move( socket ) ),
    _id( id ),
    _user( std::nullopt ),
    _last_descriptor( 0 ),
    _state( State::WAITING ),
    _filesystem( std::nullopt )
{

    _thread = std::thread( &Session::_command_loop, this );

}


void Session::_command_loop() {

    // Run the command loop.
    try {

        while ( !_finished.load( std::memory_order_relaxed ) && _socket ) {


            // COMMAND REQUEST

            // Set state flag to WAITING signalizing session is currenting listening for command from client.
            _state = State::WAITING;

            // Receive first part of command request: magic value used for desynchronization detection.
            if ( _socket.recvo<dword_t>() != protocol::sync_watchdog ) {

                throw std::runtime_error( "desynchronization detected" );

            }

            // Receive second part: command ID.
            const word_t command = _socket.recvo<word_t>();

            // Set state flag to PROCESSING signalizing session is currently doing something.
            _state = State::PROCESSING;


            // COMMAND RESPONSE

            // Check if command ID exists in server's commands register. If is reject command.
            if ( !_parent._command_registry.exists( command ) ) {

                _socket.sende( CommandResult::UNKNOWN );

            }

            // Else check if command is member of `allowed_always` list. If is accept command.
            else if ( allowed_always.find( static_cast<Command>( command ) ) != allowed_always.cend() ) {

                _socket.sende( CommandResult::OK );
                const Command alias = static_cast<Command>( command );

                switch ( alias ) {

                    case Command::NONE:
                        break;

                    case Command::BYE: {

                        _finished = true;
                        break;

                    }

                    case Command::SERVER_INFO: {

                        ServerInfoTable buffer;

                        {

                            std::shared_lock lock( _parent._info_table_mutex );

                            buffer = _parent._info_table;

                        }

                        _socket.sendo( static_cast<word_t>( buffer.size() ) );
                        for ( const auto& row : buffer ) {

                            _socket.sends( row.first );
                            _socket.sends( row.second );

                        }

                        break;
                        
                    }

                    case Command::COMMANDS: {

                        _socket.sendo( _parent._command_registry.size() );

                        for ( const auto& cmd : _parent._command_registry.get_command_list() ) {

                            _socket.sendo( cmd.first );
                            _socket.sends( cmd.second );

                        }

                        break;

                    }

                    case Command::DESCRIPTORS: {

                        std::shared_lock lock( _descriptors_mutex );

                        _socket.sendo( static_cast<dword_t>( _descriptors.size() ) );

                        for ( const auto& descriptor : _descriptors ) {

                            _socket.sendo( descriptor.first );

                        }

                        break;

                    }

                    case Command::AUTH: {

                        const std::string username = _socket.recvss();
                        const std::string password = _socket.recvss();

                        const utils::Credentials credentials( username, password );

                        {

                            std::shared_lock lock( _parent._users_limit_mutex );

                            const auto& users_limit = _parent._users_limit;

                            // Check user limit if is set.
                            if ( users_limit.find( credentials.get_user() ) != users_limit.end() ) {

                                const auto current_connections_count = _parent.get_users_session_count( credentials.get_user() );

                                if ( current_connections_count >= users_limit.at( credentials.get_user() ) ) {

                                    _socket.sende( AuthResult::USER_LIMIT_REACHED );

                                    break;

                                }

                            }

                        }

                        std::string returned_username;
                        path_t root_directory;
                        std::optional<path_t> home_directory = std::nullopt;
                        AuthResult auth_result;

                        try {

                            // Run authorization middleware.
                            auth_result = _parent._auth_middleware( credentials, returned_username, root_directory, home_directory );

                        }
                        catch ( const std::exception& e ) {

                            _socket.sende( AuthResult::MIDDLEWARE_ERROR );

                            break;

                        }

                        std::unique_lock lock( _user_mutex );

                        if ( auth_result == AuthResult::OK ) {

                            if ( returned_username.empty() ) {

                                _socket.sende( AuthResult::MIDDLEWARE_ERROR );

                                break;

                            }

                            if ( !root_directory.is_absolute() || !std::filesystem::is_directory( root_directory ) ) {

                                _socket.sende( AuthResult::MIDDLEWARE_ERROR );

                                break;

                            }

                            try {

                                VirtualFilesystem vfs( root_directory );

                                if ( home_directory ) {

                                    vfs.set_home( home_directory.value() );

                                }

                                _filesystem = vfs;

                            }
                            catch ( ... ) {

                                _socket.sende( AuthResult::MIDDLEWARE_ERROR );

                                break;

                            }

                            if ( _user ) {

                                std::unique_lock descriptors_lock( _descriptors_mutex );

                                _descriptors.clear();

                            }

                            _user = returned_username;

                        }

                        _socket.sende( auth_result );

                        if ( auth_result == AuthResult::OK ) {

                            _socket.sends( returned_username );

                        }

                        break;

                    }

                    case Command::CLEAR: {

                        std::unique_lock descriptors_lock( _descriptors_mutex );
                        std::unique_lock user_lock( _user_mutex );

                        _descriptors.clear();
                        _user.reset();
                        _filesystem.reset();

                        break;

                    }

                }

            }

            // Else (command exists in server's registry and isn't in `allowed_always`) let the command middleware decide.
            else {

                CommandResult command_result;

                // If command middleware is not set send error.
                if ( !_parent._command_middleware ) {

                    _socket.sende( CommandResult::MIDDLEWARE_ERROR );

                    continue;

                }

                // Run the command middleware.
                try {

                    command_result = _parent._command_middleware( command, _user );

                }

                // Send error to client and continue command loop.
                catch ( const std::exception& e ) {

                    _socket.sende( CommandResult::MIDDLEWARE_ERROR );

                    continue;

                }

                // Send result to client.
                _socket.sende( command_result );

                // Run command only when command middleware equals OK.
                if ( command_result == CommandResult::OK ) {

                    _parent._command_registry[ command ]( *this, _socket );

                }

            }

        }

    }

    // Log error and set finished flag on.
    catch ( const std::exception& e ) {

        temp_log( "Session #" + std::to_string( _id ) + " closed by: " + e.what() );

    }

    _finished = true;

}


std::optional<std::string> Session::get_user() const noexcept {

    std::shared_lock lock( _user_mutex );

    return _user;

}


std::set<descriptor_t> Session::get_descriptors() const noexcept {

    std::set<descriptor_t> buffer;

    std::shared_lock lock( _descriptors_mutex );

    for ( const auto& descriptor : _descriptors ) {

        buffer.emplace( descriptor.first );

    }

    return buffer;

}


void Session::close( bool clean ) {

    _finished = true;

    // Close socket first when state is WAITING (because session is currently hanged on waiting for
    // command request) or when clean argument is false (because that means we want to kill session immediately)
    if ( _state == State::WAITING || clean == false ) {

        _socket.close();

    }

    // Here the session is either in processing mode, which means it will end itself after
    // the currently processed command, or it has a closed socket, so it will also end, so we can use join().
    if ( _thread.joinable() ) {

        _thread.join();

    }

}


Session::~Session() {

    close( false );

}