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

        info( strbool( client.login( { "lol", "xd" } ) == LoginResult::OK ) );

        info( client.pwd() );
        info( client.cd( "Projects" ) );
        info( client.cd( "sfap" ) );
        info( client.cd( "build" ) );

        for ( const auto& entry : client.ls( ".", true, true, true, true ) ) {

            std::cout << "File: " << entry.path << std::endl << std::endl;
        
            if ( entry.stat ) {

                std::cout << "Size: " << entry.stat->size << std::endl;
                std::cout << "Type: " << ( int )entry.stat->type << std::endl;
                std::cout << std::endl;

            }

            if ( entry.magic ) {

                std::cout << "Description:" << std::endl;
                for ( const auto& s : entry.magic->description ) std::cout << s << std::endl;
                std::cout << std::endl;

                std::cout << "MIME:" << std::endl;
                for ( const auto& s : entry.magic->mime ) std::cout << s << std::endl;
                std::cout << std::endl;

                std::cout << "Encoding:" << std::endl;
                for ( const auto& s : entry.magic->encoding ) std::cout << s << std::endl;
                std::cout << std::endl;

            }

            std::cout << std::endl;

        }

        const auto id = client.open_file( "test.txt", std::ios::out );

        std::cout << strbool( client.eof( id ) ) << std::endl;
        std::cout << strbool( client.good( id ) ) << std::endl;
        std::cout << client.gcount( id ) << std::endl;

        client.close_file( id );

    }
    catch ( const std::exception& e ) {

        critical( e.what() );

    }

    return EXIT_SUCCESS;

}