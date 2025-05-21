#ifdef _WIN32

    #include <winsock2.h>

#else

    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <unistd.h>

#endif

#include <gtest/gtest.h>

#include <net/address/detect.hpp>
#include <net/address/host.hpp>
#include <net/iosocket/iosocket.hpp>


using namespace sfap;
using namespace sfap::net;


TEST( IOSocket, SimpleHTTP ) {

    #ifdef _WIN32

        WSADATA wsaData;
        if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 ) FAIL() << "WSAStartup failed";

    #endif

    sockaddr_storage address;
    EXPECT_NO_THROW( address = Host( "api.ipify.org", 80 ).to_native() );

    socket_t fd = socket( address.ss_family, SOCK_STREAM, IPPROTO_TCP );
    EXPECT_FALSE( fd == INVALID_SOCKET );

    if ( connect( fd, reinterpret_cast<const sockaddr*>( &address ), sizeof( address ) ) != 0 ) {

        perror( "connect failed" );
        FAIL() << "connect() failed";

    }

    EXPECT_NO_THROW(

        IOSocket client( fd );

        const std::string request =

            "GET / HTTP/1.0\r\n"
            "Host: api.ipify.org\r\n"
            "User-Agent: 007\r\n"
            "Accept: */*\r\n"
            "\r\n"

        ;

        client.send( request.data(), static_cast<dword_t>( request.size() ) );

        try {

            while ( true ) std::cout << client.recvo<char>() << std::flush;

        }
        catch ( ... ) {

            std::cout << '\n' << std::endl;

        }

        std::cout << "Sent: " << client.get_sent_bytes_count() << " Bytes" << std::endl;
        std::cout << "Received: " << client.get_received_bytes_count() << " Bytes" << std::endl;

    );

}


TEST( IOSocket, SimpleHTTPS ) {

    sockaddr_storage address;
    EXPECT_NO_THROW( address = Host( "api.ipify.org", 443 ).to_native() );

    socket_t fd = socket( address.ss_family, SOCK_STREAM, IPPROTO_TCP );
    EXPECT_FALSE( fd == INVALID_SOCKET );

    if ( connect( fd, reinterpret_cast<const sockaddr*>( &address ), sizeof( address ) ) != 0 ) {

        perror( "connect failed" );
        FAIL() << "connect() failed";

    }

    crypto::ssl_ptr ssl( SSL_new( crypto::TLSContext::default_client_context.get() ), &SSL_free );
    EXPECT_TRUE( ssl != nullptr );

    
    SSL_set_fd( ssl.get(), fd );
    SSL_set_tlsext_host_name( ssl.get(), "api.ipify.org" );

    SSL_connect( ssl.get() );

    const long result = SSL_get_verify_result( ssl.get() );
    
    if ( result != X509_V_OK ) {

        std::cerr << "verify failed: " << X509_verify_cert_error_string( result ) << std::endl;
        FAIL();

    }

    EXPECT_NO_THROW(

        IOSocket client( fd, std::move( ssl ) );

        const std::string request =

            "GET / HTTP/1.0\r\n"
            "Host: api.ipify.org\r\n"
            "User-Agent: 007\r\n"
            "Accept: */*\r\n"
            "\r\n"

        ;

        client.send( request.data(), static_cast<dword_t>( request.size() ) );

        try {

            while ( true ) std::cout << client.recvo<char>() << std::flush;

        }
        catch ( ... ) {

            std::cout << '\n' << std::endl;

        }

        std::cout << "Sent: " << client.get_sent_bytes_count() << " Bytes" << std::endl;
        std::cout << "Received: " << client.get_received_bytes_count() << " Bytes" << std::endl;

    );

}