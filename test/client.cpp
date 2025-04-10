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

        const auto info = client.space( "Projects/sfap/VERSION" );

        std::cout << info.available / 1024 << std::endl;
        std::cout << info.capacity / 1024 << std::endl;
        std::cout << info.free / 1024 << std::endl;

    }
    catch ( const std::exception& e ) {

        critical( e.what() );

    }

    return EXIT_SUCCESS;

}