#include <gtest/gtest.h>
#include <thread>
#include <chrono>

#include <net/listener/listener.hpp>
#include <net/connect/connect.hpp>
#include <net/iosocket/iosocket.hpp>

using namespace sfap;
using namespace sfap::net;

TEST( Listener, AcceptAndResponds ) {

    #ifdef _WIN32

        WSADATA wsaData;
        if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 ) FAIL() << "WSAStartup failed";

    #endif

    const Address server_address( Host( "127.0.0.1:0" ) );
    const Listener listener( server_address );

    const Host real_host = listener.get_host();
    const Address client_address( real_host );

    std::thread server_thread( [&listener]() {

        try {

            const IOSocket socket = listener.accept();
            const std::string msg = socket.recvss();
            EXPECT_EQ( msg, "ping" );
            socket.sends( "pong" );

        } catch ( const std::exception& e ) {

            FAIL() << "Server error: " << e.what();

        }

    });

    std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

    try {

        const IOSocket sock = connect( client_address );
        sock.sends( "ping" );
        const std::string response = sock.recvss();
        EXPECT_EQ( response, "pong" );

    } catch ( const std::exception& e ) {

        FAIL() << "Client error: " << e.what();

    }

    server_thread.join();
}


TEST( Listener, SSLAcceptAndResponds ) {

    const path_t cert_encrypted_file = "data/cert_encrypted.pem";
    const path_t key_encrypted_file = "data/key_encrypted.pem";

    const std::string key = "sfap";

    crypto::TLSContext server_context( SSL_VERIFY_NONE );
    crypto::TLSContext client_context( SSL_VERIFY_NONE );

    server_context.load_cert_file( cert_encrypted_file );
    server_context.load_key_file( key_encrypted_file, key );

    const Address server_address( Host( "127.0.0.1:0" ), server_context );
    const Listener listener( server_address );

    const Host real_host = listener.get_host();

    const Address client_address( real_host, client_context );

    std::thread server_thread( [&listener]() {

        try {

            const IOSocket socket = listener.accept();
            const std::string msg = socket.recvss();
            EXPECT_EQ( msg, "ping" );
            socket.sends( "pong" );

        } catch ( const std::exception& e ) {

            FAIL() << "Server error: " << e.what();

        }

    });

    std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

    try {

        const IOSocket sock = connect( client_address );
        sock.sends( "ping" );
        const std::string response = sock.recvss();
        EXPECT_EQ( response, "pong" );

    } catch ( const std::exception& e ) {

        FAIL() << "Client error: " << e.what();

    }

    server_thread.join();
}
