#include <mutex>
#include <fstream>

#include <server/command/command.hpp>
#include <server/session/session.hpp>
#include <server/procedures/procedures.hpp>
#include <protocol/protocol.hpp>
#include <utils/misc.hpp>


using namespace sfap;
using namespace sfap::net;
using namespace sfap::protocol;


bool Procedures::_loaded = false;


void Procedures::load() {

    if ( _loaded ) return;


    Commands::add_command( CommandList::NONE, "none", []( Session&, const IOSocket& ){

        // this procedure does nothing

    });

    Commands::add_command( CommandList::BYE, "bye", []( Session& session, const IOSocket& ){

        session.shutdown();

    });

    Commands::add_command( CommandList::INFO, "info", []( Session& session, const IOSocket& sock ){

        sock.sends( config::version );
        sock.sendsv( { "dupa", "chuj" } );
        sock.sendo( utils::timestamp() );

    });


    _user_group();
    _fs_group();
    //_fs_mnpl_group();
    //_ftp_group();
    _file_io_group();
    //_admin_group();

    _loaded = true;

}