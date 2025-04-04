#include <mutex>
#include <fstream>

#include <server/command/command.hpp>
#include <server/session/session.hpp>
#include <server/server.hpp>
#include <protocol/protocol.hpp>

using namespace sfap;
using namespace sfap::protocol;


void load_procedures() {

    static bool loaded = false;

    if ( loaded ) return;


    // NONE
    Commands::add_command( CommandList::NONE , "none", []( Session& ) {

        // This command does nothing

    });


    // BYE
    Commands::add_command( CommandList::BYE, "bye", []( Session& session ) {

        session.shutdown = true;

    });


    
    loaded = true;

}