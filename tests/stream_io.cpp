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

    EXPECT_NO_THROW( client.open_descriptor( "test.txt", std::ios::out | std::ios::binary ) ); 
    EXPECT_NO_THROW( client.open_descriptor( "test2.txt", std::ios::out ) );
    EXPECT_NO_THROW( client.open_descriptor( "śżółćźżćźżąą.txt", std::ios::out ) );

    EXPECT_EQ( client.get_descriptors().size(), 3 );

    EXPECT_NO_THROW( EXPECT_EQ( client.write( 1, "sfapisthebest\n", 14 ), 14 ) );

    EXPECT_NO_THROW( client.flush( 1 ) );

    client.clear();

    EXPECT_EQ( client.get_descriptors().size(), 0 );

    EXPECT_EQ( (int)client.authorize( std::make_shared<const utils::Credentials>( "sfap", "isthebest" ) ), (int)protocol::AuthResult::OK );
        
    const auto descriptor = client.open_descriptor( "test.txt", std::ios::in | std::ios::binary );
    data_t payload1( 4 );
    const data_t reference1 = {'s', 'f', 'a', 'p'};
    
    EXPECT_EQ( client.read( descriptor, payload1.data(), payload1.size() ), 4 );

    EXPECT_EQ( payload1, reference1 );

    const auto& state1 = client.iostate( descriptor );

    EXPECT_FALSE( state1.fail() );
    EXPECT_FALSE( state1.bad() );
    EXPECT_FALSE( state1.eof() );

    data_t payload2( 9 );
    const data_t reference2 = {'i', 's', 't', 'h', 'e', 'b', 'e', 's', 't'};

    EXPECT_EQ( client.read( descriptor, payload2.data(), payload2.size() ), 9 );

    EXPECT_EQ( payload2, reference2 );

    const auto& state2 = client.iostate( descriptor );

    EXPECT_FALSE( state2.fail() );
    EXPECT_FALSE( state2.bad() );
    EXPECT_FALSE( state2.eof() );

    EXPECT_EQ( client.seekg( descriptor, std::streampos( 4 ) ), std::streampos( 4 ) );

    EXPECT_EQ( client.read( descriptor, payload2.data(), payload2.size() ), 9 );

    EXPECT_EQ( payload2, reference2 );

    EXPECT_EQ( client.read( descriptor, payload2.data(), payload2.size() ), 1 );

    const auto& state3 = client.iostate( descriptor );

    EXPECT_FALSE( state3.fail() );
    EXPECT_FALSE( state3.bad() );
    EXPECT_TRUE( state3.eof() );

}