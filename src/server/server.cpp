#include <server/server.hpp>
#include <server/procedures/procedures.hpp>


using namespace sfap;
using namespace sfap::protocol;


extern CommandResult default_command_middleware( CommandList, const std::string&, const bool& );
extern LoginResult default_login_middleware( const Credentials&, std::string&, path_t&, path_t& );
extern ConnectionResult default_connection_middleware( const net::Host& host, const std::string& version );


Server::Server( const net::Address& address ) :
    _listener( address ),
    _shutdown( false ),
    command_middleware( default_command_middleware ),
    login_middleware( default_login_middleware ),
    connection_middleware( default_connection_middleware ),
    magic( std::nullopt ) {

        Procedures::load();

        _listener_thread = std::thread( [this](){ _listener_loop(); } );

}


void Server::_listener_loop() {

    while ( !_shutdown ) {

        auto client = _listener.accept();

        const std::string client_version = client.recvss();
        const net::Host client_host = client.get_remote_host();

        const auto result = connection_middleware( client_host, client_version );

        client.sendo( result );

        if ( result == ConnectionResult::OK ) _session_list.emplace_back( std::make_unique<protocol::Session>( std::move( client ), *this ) );

    }
    
}


void Server::join() {

    if ( _listener_thread.joinable() ) _listener_thread.join();

}