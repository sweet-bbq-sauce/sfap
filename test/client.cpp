#include <iostream>
#include <thread>
#include <functional>
#include <fstream>
#include <random>
#include <thread>

#include <client/client.hpp>
#include <utils/log.hpp>
#include <utils/magic/magic.hpp>


using namespace sfap;
using namespace sfap::utils;
using namespace sfap::protocol;


int main() {

    sfap::init();
    auto client = Client( net::Address( "hawai.vpn:6767", crypto::SSLContext::default_client_context ) );

    try {

        client.login( { "fds", "Fdsf" } );

        const auto id = client.open_file( "test.txt", std::ios::in | std::ios::out );

        std::cout << strbool( client.eof( id ) ) << std::endl;
        std::cout << strbool( client.good( id ) ) << std::endl;
        std::cout << client.gcount( id ) << std::endl;

        char buf[256] = { 0 };

        client.read( id, buf, sizeof(buf) );

        std::cout << "received " << client.gcount( id ) << std::endl;
        std::cout << "received " << client.gcount( id, false ) << std::endl;
        std::cout << buf << std::endl;

        client.close_file( id );

    }
    catch ( const std::exception& e ) {

        critical( e.what() );

    }

    return EXIT_SUCCESS;

}