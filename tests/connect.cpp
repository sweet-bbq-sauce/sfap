#include <gtest/gtest.h>

#include <crypto/context/context.hpp>
#include <net/address/address.hpp>
#include <net/connect/connect.hpp>


using namespace sfap;
using namespace sfap::net;


TEST( Connect, SimpleConnect ) {

    #ifdef _WIN32

        WSADATA wsaData;
        if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 ) FAIL() << "WSAStartup failed";

    #endif

    const Host target( "api.ipify.org:443" );
    const utils::Credentials credentials( "sfap", "isthebest" );
    const Address address(

        target,
        crypto::TLSContext::default_client_context,
        {

            Proxy( proxy_type::SOCKS5, Host( "127.0.0.1:42000" ), credentials ),
            Proxy( proxy_type::HTTP_CONNECT, Host( "127.0.0.1:42001" ), credentials ),
            Proxy( proxy_type::SOCKS5, Host( "127.0.0.1:42000" ), credentials )

        }

    );

    EXPECT_NO_THROW(
        
        const auto sock = net::connect( address );

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

    );

}