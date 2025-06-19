/*!
 *  \file
 *  \brief Source file containing `Client` class definition.
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


#include <stdexcept>

#include <client/client.hpp>
#include <client/file_info.hpp>
#include <net/address/address.hpp>
#include <net/connect/connect.hpp>
#include <net/iosocket/iosocket.hpp>
#include <protocol/protocol.hpp>
#include <utils/credentials.hpp>


using namespace sfap;
using namespace sfap::protocol;


Client::Client( const net::Address& address, std::shared_ptr<const utils::Credentials> credentials ) :
    _socket( net::connect( address ) ),
    _address( address ),
    _username( std::nullopt)
{
    
    if ( credentials ) {

        authorize( credentials );

    }

}


void Client::noop() const {

    _request_command( Command::NONE );

}


void Client::close() {

    if ( is_opened() ) {

        _request_command( Command::BYE );

    }

    _socket.close();

}


protocol::AuthResult Client::authorize( std::shared_ptr<const utils::Credentials> credentials ) {

    if ( !credentials ) {

        throw std::invalid_argument( "credentials is null" );

    }

    _request_command( Command::AUTH );

    _socket.sends( credentials->get_user() );
    _socket.sends( credentials->get_password() );

    const auto auth_result = _socket.recve<AuthResult>();

    if ( auth_result == AuthResult::OK ) {

        _credentials = credentials;
        _username = _socket.recvss();
        _cache.home = _socket.recvp();
        _cache.cwd = _socket.recvp();

    }

    return auth_result;

}


void Client::clear() {

    _request_command( Command::CLEAR );

    _username.reset();
    _credentials.reset();

}


const std::optional<std::string>& Client::get_username() const noexcept {

    return _username;

}


ServerCommandsTable Client::get_server_commands() const {

    ServerCommandsTable buffer;

    _request_command( Command::COMMANDS );

    const word_t list_size = _socket.recvo<word_t>();
    
    for ( word_t i = 0; i < list_size; i++ ) {

        const auto id = _socket.recvo<word_t>();
        const auto name = _socket.recvss();

        buffer[id] = name;

    }

    return buffer;

}


ServerInfoTable Client::get_server_info() const {

    ServerInfoTable buffer;

    _request_command( Command::SERVER_INFO );

    const word_t list_size = _socket.recvo<word_t>();

    for ( word_t i = 0; i < list_size; i++ ) {

        const auto key = _socket.recvss();
        const auto value = _socket.recvss();

        buffer[key] = value;

    }

    return buffer;

}


bool Client::is_opened() const noexcept {

    return _socket.is_open();

}


Client::operator bool() const noexcept {

    return is_opened();

}


bool Client::is_authorized() const noexcept {

    return _credentials.operator bool() && _username;

}


bool Client::is_secure() const noexcept {

    return _socket.is_secure();

}


Client Client::clone() const {

    return Client( _address, _credentials );

}


void Client::_request_command( word_t command ) const {

    if ( !is_opened() ) {

        throw std::logic_error( "client is closed" );

    }

    _socket.sendo( sync_watchdog );
    _socket.sendo( command );

    const auto command_result = _socket.recve<CommandResult>();

    if ( command_result != CommandResult::OK ) {

        switch ( command_result ) {

            case CommandResult::ACCESS_DENIED:
                throw std::runtime_error( "command is denied by the server's command middleware" );

            case CommandResult::UNAVAILABLE:
                throw std::runtime_error( "command is temporarily unavailable" );

            case CommandResult::DISABLED:
                throw std::runtime_error( "command is permanently disabled by server configuration" );

            case CommandResult::UNSUPPORTED:
                throw std::runtime_error( "command is not supported on this server" );

            case CommandResult::MIDDLEWARE_ERROR:
                throw std::runtime_error( "an exception occurred in the command's middleware layer" );

            case CommandResult::UNKNOWN:
                throw std::runtime_error( "command does not exist in the server's command registry" );

        }

    }

}


void Client::_request_command( Command command ) const {

    _request_command( static_cast<word_t>( command ) );

}


Client::~Client() {

    close();

}


path_t Client::cd( const path_t& path ) const {

    _request_command( Command::CD );

    _socket.sendp( path );

    const auto result = _socket.recve<AccessResult>();

    if ( result == AccessResult::OK ) {

        _cache.cwd = _socket.recvp();

        return _cache.cwd;

    }
    else {

        throw std::runtime_error( "CD returned error: " + std::to_string( (int)result ) );

    }

}


path_t Client::pwd( bool use_cache ) const {

    if ( !use_cache ) {

        _request_command( Command::PWD );

        _cache.cwd = _socket.recvp();

    }

    return _cache.cwd;

}


path_t Client::home( bool use_cache ) const {

    if ( !use_cache ) {

        _request_command( Command::HOME );

        _cache.home = _socket.recvp();

    }

    return _cache.home;

}


std::vector<utils::FileInfo> Client::ls( const path_t& path ) const {

    _request_command( Command::LS );

    _socket.sendp( path );

    const auto result = _socket.recve<AccessResult>();

    if ( result != AccessResult::OK ) {

        throw std::runtime_error( "LS returned error: " + std::to_string( (int)result ) );

    }

    const auto size = _socket.recvo<dword_t>();
    std::vector<utils::FileInfo> buffer;

    for ( dword_t i = 0; i < size; i++ ) {

        utils::FileInfo entry;

        entry._type = _socket.recve<utils::FileInfo::type>();
        entry._path = _socket.recvp();
        entry._size = _socket.recvo<qword_t>();

        buffer.push_back( entry );

    }

    return buffer;

}