#include <server/procedures/procedures.hpp>
#include <server/session/session.hpp>
#include <net/iosocket/iosocket.hpp>
#include <server/command/command.hpp>
#include <server/server.hpp>


using namespace sfap;
using namespace sfap::net;
using namespace sfap::protocol;


void Procedures::_user_group() {


    Commands::add_command( CommandList::WHOAMI, "whoami", []( Session& session, const IOSocket& sock ) {

        sock.sends( session.username );

    });


    Commands::add_command( CommandList::HOME, "home", []( Session& session, const IOSocket& sock ) {

        sock.sends( session.userify( session.home_directory ).string() );

    });


    Commands::add_command( CommandList::LOGIN, "login", []( Session& session, const IOSocket& sock ) {

        const std::string login = sock.recvss();
        const std::string password = sock.recvss();

        const auto result = session.parent.login_middleware( { login, password }, session.username, session.root_directory, session.home_directory );

        sock.sendo( result );

        if ( result == LoginResult::OK ) {

            session.logged = true;
            session.cwd_directory = session.home_directory;

        }

    });


    Commands::add_command( CommandList::LOGOUT, "logout", []( Session& session, const IOSocket& sock ) {

        session.logged = false;
        session.username = default_username;

        session.root_directory.clear();
        session.home_directory.clear();
        session.cwd_directory.clear();

    });


    Commands::add_command( CommandList::IS_LOGGED, "is_logged", []( Session& session, const IOSocket& sock ) {

        sock.sendo( session.logged );

    });


}