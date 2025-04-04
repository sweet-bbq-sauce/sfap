#include <iostream>


#include <server/server.hpp>
#include <utils/format.hpp>
#include <sfap.hpp>
#include <crypto/hash.hpp>
#include <utils/log.hpp>
#include <utils/crc.hpp>
#include <protocol/transfer/transfer.hpp>


using namespace sfap;
using namespace sfap::utils;
using namespace sfap::protocol;


int main() {

    init();

    const auto ssl = std::make_shared<crypto::SSLContext>( ( path_t )"../cert/key.pem", ( path_t )"../cert/cert.pem" );
    net::Listener server( net::Address( net::Host( "0.0.0.0:6767" ), ssl ) );
    const auto client = server.accept();

    receive_file( client, "test.bin" );

    return EXIT_SUCCESS;

}