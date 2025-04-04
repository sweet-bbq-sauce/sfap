#include <protocol/transfer/transfer.hpp>
#include <net/iosocket/iosocket.hpp>
#include <utils/log.hpp>
#include <utils/format.hpp>


using namespace sfap;
using namespace sfap::protocol;


void protocol::receive_data( const net::IOSocket& sock, void* data, qword_t size, const TransferOptions& options ) {

    // We don't do anything, size is given from client and succes flag is sent.
    if ( size == 0 ) return;

    // There we should check if we have enough space but now we blindly send OK
    sock.sendo( true );

    if ( options.chunk_size == 0 ) {

        // Get whole data at once
        sock.recv( data, size );

        // If we're using hash, we're check it at the end
        if ( options.hash_algorithm != crypto::HashAlgorithm::NONE ) {

            data_t local_hash, remote_hash;

            // Calculate hash of the received data
            local_hash = crypto::hash_data( options.hash_algorithm, data, size );

            // Get clients hash to compare
            sock.recvd( remote_hash );

            // Send result to the client
            sock.sendo( local_hash == remote_hash );

        }
        else sock.sendo( true );

    }
    else {

        crypto::Hash local_hash( options.hash_algorithm );

        qword_t received = 0;

        while ( received < size ) {

            void* destination = static_cast<byte_t*>( data ) + received;
            const qword_t chunk_size = sock.recvo<qword_t>();

            sock.recv( destination, chunk_size );

            const crc_t remote_crc = sock.recvo<crc_t>();
            const crc_t local_crc = utils::CRC32::calculate( destination, chunk_size );

            if ( local_crc == remote_crc ) {

                sock.sendo( true );
                if ( local_hash ) local_hash.update( destination, chunk_size );
                received += chunk_size;

            }
            else sock.sendo( false );

        }

        if ( local_hash ) {

            data_t remote_hash;

            sock.recvd( remote_hash );

            if ( remote_hash == local_hash.finish() ) sock.sendo( true );
            else {

                sock.sendo( false );
                utils::error( "Transfer fault: hash doesn't match" );

            }

        }
        else sock.sendo( true );

    }

}


void protocol::receive_data( const net::IOSocket& sock, data_t& data, const TransferOptions& options ) {

    receive_data( sock, data.data(), data.size(), options );

}