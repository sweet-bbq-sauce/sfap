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


using namespace sfap;
using namespace sfap::protocol;


Server::Server( const net::Address& address, const CommandRegistry& command_registry ) :
    _listener( address ),
    _running( true ),
    _command_registry( command_registry ),
    _finished_sessions( 0 ),
    _auth_middleware( default_authorize_middleware ),
    _command_middleware( default_command_middleware )
{

    set_info_table( {} );

    _listener_thread = std::thread( [this]() {

        session_id_t current_id = 0;

        while ( _running ) {

            try {

                current_id++;

                auto client = _listener.accept();

                if ( !client ) {

                    continue;

                }

                std::unique_lock lock( _sessions_mutex );

                _sessions.emplace( current_id, std::make_unique<Session>( client, *this, current_id ) );
            
            }
            catch ( const std::exception& e ) {

                std::cerr << "error in listener loop: " << e.what() << std::endl;

            }

        }

        _running = false;

    });

    _cleaner_thread = std::thread( [this]( std::chrono::milliseconds delay ) {

        std::unique_lock lock( _cleaner_mutex );

        while ( _running ) {

            _cleaner_cv.wait_for( lock, delay, [this]() {

                return !_running;

            });

            if ( !_running ) {

                break;

            }

            std::unique_lock lock( _sessions_mutex );

            std::vector<session_id_t> to_erase;

            for ( const auto& session : _sessions ) {

                if ( session.second->_finished == true ) {

                    to_erase.push_back( session.first );

                }

            }

            _finished_sessions.fetch_add( to_erase.size(), std::memory_order_relaxed );

            for ( const session_id_t& session : to_erase ) {

                _sessions.erase( session );

            }

        }

    }, std::chrono::milliseconds( 1000 ) );

}


void Server::set_info_table( const ServerInfoTable& info_table ) {

    std::unique_lock lock( _info_table_mutex );

    _info_table = info_table;
    _info_table["version"] = LIBSFAP_VERSION;

}


void Server::set_command_middleware( const CommandMiddleware& command_middleware ) {

    if ( command_middleware == nullptr ) {

        throw std::invalid_argument( "command middleware is null" );

    }

    _command_middleware = command_middleware;

}


void Server::set_auth_middleware( const AuthMiddleware& auth_middleware ) {

    if ( auth_middleware == nullptr ) {

        throw std::invalid_argument( "auth middleware is null" );

    }

    _auth_middleware = auth_middleware;

}


std::size_t Server::get_session_count() const noexcept {

    std::shared_lock lock( _sessions_mutex );

    return _sessions.size();

}


std::size_t Server::get_finished_session_count() const noexcept {

    return _finished_sessions.load( std::memory_order_relaxed );

}


std::size_t Server::get_users_session_count( const std::string& user ) const noexcept {

    std::size_t count = 0;

    std::shared_lock lock( _sessions_mutex );

    for ( const auto& session : _sessions ) {

        if ( session.second->get_user() == user ) {

            count++;

        }

    }

    return count;

}


void Server::set_user_sessions_limit( const std::string& user, std::optional<std::size_t> limit ) {

    if ( user.empty() ) {

        throw std::invalid_argument( "user is empty" );

    }

    std::unique_lock lock( _users_limit_mutex );

    if ( limit ) {

        _users_limit[user] = limit.value();

    }
    else {

        _users_limit.erase( user );

    }

}


void Server::stop_accepting() {

    _running = false;
    _listener.close();

}


void Server::close() {

    stop_accepting();

    _cleaner_cv.notify_all();

    if ( _listener_thread.joinable() ) {

        _listener_thread.join();

    }

    if ( _cleaner_thread.joinable() ) {

        _cleaner_thread.join();

    }

    std::unique_lock lock( _sessions_mutex );
    _sessions.clear();

}


Server::~Server() {

    close();

}


void Server::hang_on() {

    if ( _listener_thread.joinable() ) {

        _listener_thread.join();

    }

    if ( _cleaner_thread.joinable() ) {

        _cleaner_thread.join();

    }

}


bool Server::is_open() const noexcept {

    return _listener.is_open();

}


std::size_t Server::get_all_sessions_count() const noexcept {

    return _finished_sessions;

}