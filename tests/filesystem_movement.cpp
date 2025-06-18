#include <gtest/gtest.h>

#include <net/address/address.hpp>
#include <server/server.hpp>
#include <client/client.hpp>


using namespace sfap;
using namespace sfap::net;


TEST( FilesystemMovement, CheckCommands ) {

    #ifdef _WIN32

        WSADATA wsaData;
        if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 ) FAIL() << "WSAStartup failed";

    #endif

    const Address server_address( Host( "127.0.0.1:6666" ) );

    Server server( server_address );

    server.set_auth_middleware( []( const utils::Credentials&, std::string& username, path_t& root, std::optional<path_t>& home ) {

        username = "doesn't matter what but must be set";

        root = std::filesystem::current_path();
        home = path_t( "~/src" );

        return protocol::AuthResult::OK;

    });

    // Wait for server is started.
    std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

    auto credentials = std::make_shared<const utils::Credentials>( "sfap", "isthebest" );

    Client client( server_address, credentials );

    EXPECT_EQ( client.pwd(), path_t( "/src" ) );
    EXPECT_EQ( client.home(), path_t( "/src" ) );
    EXPECT_EQ( client.cd( "CMakeFiles" ), path_t( "/src/CMakeFiles" ) );
    EXPECT_EQ( client.cd( "sfap.dir" ), path_t( "/src/CMakeFiles/sfap.dir" ) );
    EXPECT_EQ( client.cd( "~/../docs" ), path_t( "/docs" ) );
    EXPECT_EQ( client.cd( "/tests" ), path_t( "/tests" ) );
    EXPECT_EQ( client.cd( "." ), path_t( "/tests" ) );
    EXPECT_EQ( client.cd( ".." ), path_t( "/" ) );

    EXPECT_ANY_THROW( client.cd( "../../../" ) );

    EXPECT_NO_THROW( client.ls() );

}