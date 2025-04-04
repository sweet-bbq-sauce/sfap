#include <iostream>
#include <thread>
#include <functional>
#include <fstream>
#include <random>

#include <zlib.h>

#include <crypto/hash.hpp>

#include <client/client.hpp>
//#include <client/remote_file/remote_file.hpp>

#include <utils/log.hpp>
#include <utils/format.hpp>
#include <utils/misc.hpp>
#include <utils/crc.hpp>
#include <protocol/transfer/transfer.hpp>


sfap::data_t generateRandomData( std::size_t size ) {

    sfap::data_t data(size);

    std::random_device rd;
    std::mt19937 gen( rd() );
    std::uniform_int_distribution<sfap::byte_t> dist( 0, UINT8_MAX );

    for ( auto& byte : data ) byte = dist( gen );

    return data;

}


using namespace sfap;
using namespace sfap::utils;
using namespace sfap::protocol;


int main() {

    sfap::init();

    const data_t buffer = generateRandomData( 268435456 );
    info( "Size: ", buffer.size(), " Bytes" );
    info( "Hash: ", to_hex( crypto::hash_data( crypto::HashAlgorithm::SHA256, buffer ) ) );

    const auto client = net::connect( net::Address( "hawai.vpn:6767", crypto::SSLContext::default_client_context ) );

    TransferOptions lol;
    lol.chunk_size = 0;
    lol.hash_algorithm = crypto::HashAlgorithm::NONE;

    send_data( client, buffer, lol );

    return EXIT_SUCCESS;

}