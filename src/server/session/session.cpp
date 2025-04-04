#include <iostream>

#include <server/session/session.hpp>
#include <server/server.hpp>
#include <server/command/command.hpp>


using namespace sfap::protocol;


Session::Session( net::IOSocket&& sock, Server& parent ) :
    sock( std::move( sock ) ),
    parent( parent ),
    shutdown( false ),
    logged( false ),
    _last_file_id( 0 ),
    username( "null" ) {

        _session_thread = std::thread( [this]() { _session_loop(); } );

    }


void Session::_session_loop() {

        try {

            while ( !shutdown ) {

                const auto command = sock.recvo<protocol::CommandList>();       // Get command ID from client

                const auto result = parent.command_middleware( command, username, logged );       // Check if client has access to the command
                sock.sendo( result );      // Return result to client

                // If result is OK run command, else continue while and wait for next command
                if ( result == protocol::CommandResult::OK ) Commands::execute( command, *this );

            }

        }

        catch ( const std::exception& e ) {

            std::cout << "Client unexpected disconnected: " << e.what() << std::endl;

            shutdown = true;

        }

        std::cout << "Closing session cleanly" << std::endl;

}