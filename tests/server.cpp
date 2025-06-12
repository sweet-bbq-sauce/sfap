#include <gtest/gtest.h>

#include <net/address/address.hpp>
#include <server/server.hpp>
#include <client/client.hpp>


using namespace sfap;
using namespace sfap::net;


TEST( Server, Constructor ) {

    #ifdef _WIN32

        WSADATA wsaData;
        if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 ) FAIL() << "WSAStartup failed";

    #endif

    const Address server_address( Host( "127.0.0.1:6666" ) );

    Server server( server_address );

    EXPECT_NO_THROW( server.set_info_table( { { "status", "dev" } } ) );

    // Wait for server is started.
    std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

    auto credentials = std::make_shared<const utils::Credentials>( "sfap", "isthebest" );

    std::thread threads[50];
    for ( auto& t : threads ) {

        t = std::thread( [&]() {

            for ( int i = 0; i < 5; i++ ) {

                EXPECT_NO_THROW(
                    
                    Client client( server_address, credentials );

                    EXPECT_TRUE( client.is_authorized() );

                    // Do something.
                    client.get_server_info();
                    client.get_server_commands();

                );

                std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

            }

        });

    }

    // Wait for all clients.
    for ( auto& t : threads ) {

        if ( t.joinable() ) {

            t.join();

        }

    }

    // Wait for cleaner iteration.
    std::this_thread::sleep_for( std::chrono::milliseconds( 1100 ) );

    EXPECT_EQ( server.get_finished_session_count(), 250 );
    EXPECT_EQ( server.get_session_count(), 0 );

    // Create authorized client.
    EXPECT_NO_THROW(

        Client client1( server_address );

        EXPECT_EQ( client1.authorize( credentials ), protocol::AuthResult::OK );

        auto client2 = client1.clone();

        client1.clear();

        EXPECT_FALSE( client1.is_authorized() );
        EXPECT_TRUE( client2.is_authorized() );

        client1.noop();

        EXPECT_TRUE( client1.get_server_commands().size() > 0 );
        EXPECT_EQ( client2.get_server_info()["status"], "dev" );

        EXPECT_FALSE( client1.is_secure() );
        EXPECT_FALSE( client2.is_secure() );

        EXPECT_TRUE( client1.is_opened() );
        EXPECT_TRUE( client2.is_opened() );

    );

}