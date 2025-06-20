#include <gtest/gtest.h>

#include <net/address/address.hpp>
#include <server/server.hpp>
#include <client/client.hpp>

#include <tests/tests.hpp>


using namespace sfap;
using namespace sfap::net;


TEST( StreamIO, OpenAndClear ) {

    #ifdef _WIN32

        WSADATA wsaData;
        if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 ) FAIL() << "WSAStartup failed";

    #endif

    const Address server_address( Host( "127.0.0.1:6666" ) );

    Server server( server_address );

    // Wait for server is started.
    std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

    Client client( server_address, std::make_shared<const utils::Credentials>( "sfap", "isthebest" ) );

    EXPECT_NO_THROW( client.open_descriptor( "test.txt", std::ios::out ) ); 
    EXPECT_NO_THROW( client.open_descriptor( "test2.txt", std::ios::out ) );
    EXPECT_NO_THROW( client.open_descriptor( "śżółćźżćźżąą.txt", std::ios::out ) );

    EXPECT_EQ( client.get_descriptors().size(), 3 );

    client.clear();

    EXPECT_EQ( client.get_descriptors().size(), 0 );

}