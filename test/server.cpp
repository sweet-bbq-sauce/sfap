#include <iostream>


#include <sfap.hpp>
#include <server/server.hpp>
#include <utils/log.hpp>
#include <utils/magic/magic.hpp>


using namespace sfap;


int main() {

    init();

    utils::Log::debug_mode = true;

    const utils::Magic magic;

    const auto ssl = std::make_shared<crypto::SSLContext>( ( path_t )"../cert/key.pem", ( path_t )"../cert/cert.pem" );
    Server server( net::Address( "0.0.0.0:6767", ssl ) );

    server.set_magic_context( magic );
    
    server.join();

    return EXIT_SUCCESS;

}