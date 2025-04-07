#include <iostream>
#include <algorithm>

#include <server/session/session.hpp>
#include <server/server.hpp>
#include <server/command/command.hpp>
#include <protocol/protocol.hpp>
#include <utils/log.hpp>


using namespace sfap::protocol;


Session::Session( net::IOSocket&& sock, Server& parent ) :
    _sock( std::move( sock ) ),
    parent( parent ),
    _shutdown( false ),
    logged( false ),
    _last_file_id( 0 ),
    username( default_username ) {

    _session_thread = std::thread( [this]() { _session_loop(); } );

}


void Session::_session_loop() {

    try {

        while ( !_shutdown ) {

            // Watchdog
            if ( _sock.recvo<dword_t>() != protocol::precommand_header ) throw std::runtime_error( "connection desynchronized" );

            const auto command = _sock.recvo<protocol::CommandList>();       // Get command ID from client

            if ( !Commands::has_command( command ) ) {

                _sock.sendo( protocol::CommandResult::COMMAND_NOT_EXISTS );

                continue;

            }

            static const std::vector<CommandList> allowed = { CommandList::NONE, CommandList::BYE, CommandList::LOGIN };

            if ( std::find( allowed.begin(), allowed.end(), command ) != allowed.end() ) {

                _sock.sendo( CommandResult::OK );

                Commands::execute( command, *this );

                continue;

            }

            const auto result = parent.command_middleware( command, username, logged );       // Check if client has access to the command
            _sock.sendo( result );      // Return result to client

            // If result is OK run command, else continue while and wait for next command
            if ( result == protocol::CommandResult::OK ) Commands::execute( command, *this );

        }

        utils::info( "Closing session cleanly" );

    }
    catch ( const std::exception& e ) {

        utils::info( "Client unexpected disconnected: ", e.what() );

        _shutdown = true;

    }

    _sock.close();

}


void Session::shutdown() noexcept {

    _shutdown = true;

}