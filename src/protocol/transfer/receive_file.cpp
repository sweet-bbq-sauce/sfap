#include <fstream>
#include <filesystem>
#include <memory>

#include <protocol/transfer/transfer.hpp>
#include <net/iosocket/iosocket.hpp>


using namespace sfap;
using namespace sfap::protocol;


void protocol::receive_file( const net::IOSocket& sock, const path_t& path ) {

    const qword_t size = sock.recvo<qword_t>();
    std::ofstream output( path, std::ios::binary );

    const auto cleanup = [&] {

        output.close();
        std::filesystem::remove(path);
    
    };

    if ( size == 0 ) sock.sendo( ( bool )output );
    else {

        const auto options = sock.recvo<TransferOptions>();

        if ( std::filesystem::space( path ).available < size || !output ) sock.sendo( false );
        else {

            sock.sendo( true );

            qword_t received = 0;
            crypto::Hash local_hash( options.hash_algorithm );

            if ( options.chunk_size == 0 ) {

                const auto buffer = std::make_unique<char[]>( config::defaults::buffer_size );

                while ( received < size ) {

                    const qword_t to_receive = std::min( ( size - received ), config::defaults::buffer_size );

                    sock.recv( buffer.get(), to_receive );

                    output.write( buffer.get(), to_receive );

                    if ( local_hash ) local_hash.update( buffer.get(), to_receive );

                    received += to_receive;

                }

            }
            else {

                auto buffer = std::make_unique<char[]>( options.chunk_size );

                while ( received < size ) {

                    const qword_t current_chunk_size = sock.recvo<qword_t>();

                    sock.recv( buffer.get(), current_chunk_size );

                    const crc_t remote_crc = sock.recvo<crc_t>();
                    const crc_t local_crc = utils::CRC32::calculate( buffer.get(), current_chunk_size );

                    if ( remote_crc == local_crc ) {

                        sock.sendo( true );

                        output.write( buffer.get(), current_chunk_size );

                        if ( local_hash ) local_hash.update( buffer.get(), current_chunk_size );

                        received += current_chunk_size;

                    }
                    else sock.sendo( false );

                }

            }

            if ( local_hash ) {

                data_t remote_hash;

                sock.recvd( remote_hash );

                if ( remote_hash == local_hash.finish() ) sock.sendo( true );
                else {

                    sock.sendo( false );

                    cleanup();

                }

            }
            else sock.sendo( true );

        }

    }

}
