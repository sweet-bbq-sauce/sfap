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

    const std::string key = "sfap";

    auto ssl_ctx = std::make_shared<crypto::TLSContext>( SSL_VERIFY_NONE );

    const path_t cert_encrypted_file = "data/cert_encrypted.pem";
    const path_t key_encrypted_file = "data/key_encrypted.pem";

    ssl_ctx->load_cert_file( cert_encrypted_file );
    ssl_ctx->load_key_file( key_encrypted_file, key );

    const Address server_address( bind_host, use_tls ? ssl_ctx : nullptr );

    Server server( server_address );

    std::cout << "Server is listening..." << std::endl;

    server.hang_on();

    return EXIT_SUCCESS;

}