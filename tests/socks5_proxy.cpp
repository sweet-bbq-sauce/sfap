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
#include <net/proxy/proxy.hpp>

#include <tests/tests.hpp>


using namespace sfap;
using namespace sfap::net;


TEST( Socks5, SimpleConnect ) {

    #ifdef _WIN32

        WSADATA wsaData;
        if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 ) FAIL() << "WSAStartup failed";

    #endif

    sockaddr_storage address;
    EXPECT_NO_THROW( address = Host( "127.0.0.1:42000" ).to_native() );

    socket_t fd = socket( address.ss_family, SOCK_STREAM, IPPROTO_TCP );
    EXPECT_FALSE( fd == INVALID_SOCKET );

    if ( connect( fd, reinterpret_cast<const sockaddr*>( &address ), sizeof( address ) ) != 0 ) {

        perror( "connect failed" );
        FAIL() << "connect() failed";

    }

    const Host target( "api.ipify.org:80" );
    const IOSocket sock( fd );
    auto cred = std::make_shared<const utils::Credentials>( "sfap", "isthebest" );

    Proxy proxy( proxy_type::SOCKS5, target, cred );

    EXPECT_NO_THROW( proxy.open( target, sock ) );

    const std::string request =

        "GET / HTTP/1.0\r\n"
        "Host: api.ipify.org\r\n"
        "User-Agent: 007\r\n"
        "Accept: */*\r\n"
        "\r\n"

    ;

    sock.send( request.data(), static_cast<dword_t>( request.size() ) );

    try {

        while ( true ) std::cout << sock.recvo<char>() << std::flush;

    }
    catch ( ... ) {

        std::cout << '\n' << std::endl;

    }

    std::cout << "Sent: " << sock.get_sent_bytes_count() << " Bytes" << std::endl;
    std::cout << "Received: " << sock.get_received_bytes_count() << " Bytes" << std::endl;

}
