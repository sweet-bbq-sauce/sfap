#include <fstream>
#include <filesystem>

#include <protocol/transfer/transfer.hpp>
#include <net/iosocket/iosocket.hpp>


using namespace sfap;
using namespace sfap::protocol;


void protocol::receive_file( const net::IOSocket& sock, const path_t& path ) {

    const qword_t size = sock.recvo<qword_t>();
    std::ofstream output( path, std::ios::binary );

    if ( size == 0 ) sock.sendo( ( bool )output );
    else {

        const auto options = sock.recvo<TransferOptions>();

        if ( std::filesystem::space( path ).available < size || !output ) sock.sendo( false );
        else {

            sock.sendo( true );

            if ( options.chunk_size == 0 ) {

                char buffer[1024 * 1024];

                qword_t received = 0;

                crypto::Hash local_hash( options.hash_algorithm );

                while ( received < size ) {

                    const qword_t to_receive = std::min( ( size - received ), sizeof( buffer ) );

                    sock.recv( buffer, to_receive );

                    output.write( buffer, to_receive );

                    if ( local_hash ) local_hash.update( buffer, to_receive );

                    received += to_receive;

                }

                if ( local_hash ) {

                    data_t remote_hash;

                    sock.recvd( remote_hash );

                    if ( remote_hash == local_hash.finish() ) sock.sendo( true );
                    else {

                        sock.sendo( false );

                        output.close();

                        std::filesystem::remove( path );

                    }

                }
                else sock.sendo( true );

            }

        }

    }

}
