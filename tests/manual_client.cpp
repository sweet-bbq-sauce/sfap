#include <net/address/address.hpp>
#include <server/server.hpp>
#include <client/client.hpp>


using namespace sfap;
using namespace sfap::net;


int main( int argn, char* argv[] ) {

    #ifdef _WIN32

        WSADATA wsaData;
        if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 ) return EXIT_FAILURE;

    #endif

    const std::string help = "usage: " + std::string( argv[0] ) + " hostname:port [tls]";

    Host bind_host;
    bool use_tls = false;

    if ( argn < 2 ) {

        std::cerr << help << std::endl;

        return EXIT_FAILURE;

    }
    else if ( argn == 3 ) {

        if ( std::string( argv[2] ) != "tls" ) {

            std::cerr << help << std::endl;

            return EXIT_FAILURE;

        }

        use_tls = true;

    }
    else if ( argn > 3 ) {

        std::cerr << help << std::endl;

        return EXIT_FAILURE;

    }

    try {

        bind_host.set_host( argv[1] );

    }
    catch ( ... ) {

        std::cerr << "invalid address format" << std::endl;
        std::cerr << help << std::endl;

        return EXIT_FAILURE;

    }

    auto ssl_ctx = std::make_shared<crypto::TLSContext>( SSL_VERIFY_NONE );

    const Address server_address( bind_host, use_tls ? ssl_ctx : nullptr );

    std::cout << "Connecting... " << std::flush;

    Client client( server_address, std::make_shared<utils::Credentials>( "sfap", "isthebest" ) );

    std::cout << "OK" << std::endl;

    std::cout << client.home() << std::endl;

    for ( const auto& entry : client.ls() ) {

        std::cout << entry.get_filename() << " | " << (int)entry.get_type() << " | " << entry.get_size() << std::endl;

    }

    return EXIT_SUCCESS;

}