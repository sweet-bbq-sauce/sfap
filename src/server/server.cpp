#include <server/server.hpp>


using namespace sfap;
using namespace sfap::protocol;


extern CommandResult default_command_middleware( CommandList, const std::string&, const bool& );
extern LoginResult default_login_middleware( const Credentials&, std::string&, bool&, path_t&, path_t& );
extern void load_procedures();


Server::Server( const net::Address& address ) :
    _listener( address ),
    _shutdown( false ),
    command_middleware( default_command_middleware ),
    login_middleware( default_login_middleware ) {

        load_procedures();

        _listener_thread = std::thread( [this](){ _listener_loop(); } );

}


void Server::_listener_loop() {

    while ( !_shutdown ) _session_list.emplace_back( std::make_unique<protocol::Session>( std::move( _listener.accept() ), *this ) );
    
}


void Server::join() {

    if ( _listener_thread.joinable() ) _listener_thread.join();

}