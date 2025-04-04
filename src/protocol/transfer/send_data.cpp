#include <protocol/transfer/transfer.hpp>
#include <net/iosocket/iosocket.hpp>
#include <utils/log.hpp>
#include <utils/format.hpp>


using namespace sfap;
using namespace sfap::protocol;


void protocol::send_data( const net::IOSocket& sock, const void* data, qword_t size, const TransferOptions& options ) {

    // size == 0 is OK, for example sending empty file. We must only send this size and it's end of this transfer
    if ( size == 0 ) {

        sock.sendo( 0 );

        // Server shouldn't deny this but he can
        if ( !sock.recvo<bool>() ) throw std::runtime_error( "server denied transfer" );

        return;

    }

    if ( !data ) throw std::invalid_argument( "data is null" );

    sock.sendo( size );     // Send whole size
    //sock.sendo( options.chunk_size );   // Send regular chunk size. If > 0 then server knows that we're using CRC
    //sock.sendo( options.hash_algorithm );   // Send hash algorithm we want to use or NONE if we don't want to use hashing
    sock.sendo( options );

    // Server now can deny for example when he doesn't have enough disk space/memory for this
    if ( !sock.recvo<bool>() ) throw std::runtime_error( "server rejected transfer at the beginning" );

    if ( options.chunk_size == 0 ) {

        // If `chunk_size` == 0 that means we don't want to use CRC. We're gonna send it as a one whole chunk, but still a chunk.
        sock.send( data, size );

        // Send hash if `hash_algorithm` option is set
        if ( options.hash_algorithm != crypto::HashAlgorithm::NONE ) sock.sendd( crypto::hash_data( options.hash_algorithm, data, size ) );

    }
    else {

        // If `chunk_size` > 0 that means we want to divide data to the chunks of the `chunk_size`, calculate CRC for every chunk and wait for server response to continue sending

        crypto::Hash local_hash( options.hash_algorithm );  // If `hash_algorithm` is NONE then (bool)local_hash is false but DON'T USE .update() OR .finish() ON THAT OBJECT !!

        qword_t sent = 0;           // Whole data counter
        bool repeating = false;     // false when we want to continue data sending, true when we want to repeat last chunk when failed

        while ( sent < size ) {

            const qword_t to_send = std::min( ( size - sent ), options.chunk_size );    // Calculate current chunk size
            const void* source = static_cast<const byte_t*>( data ) + sent;   // Calculate current chunk source position

            if ( local_hash && !repeating ) local_hash.update( source, to_send );   // Update main hash only when current chunk is sent first time
            const crc_t crc = utils::CRC32::calculate( source, to_send );   // Calculate current chunk CRC

            sock.sendb( source, to_send );      // Sent chunk data with size
            sock.sendo( crc );  // Send CRC

            // ... Server is now receiving chunk and our CRC, calculating his CRC, compares CRCs and sending to us a success bool flag ...

            if ( sock.recvo<bool>() ) {

                // If server sent true (succes), incrase `sent` counter and set `repeating` to false. We're gonne send next chunk in the next iteration
                sent += to_send;
                repeating = false;

            }
            else repeating = true;      // If server sent false (fail) that means servers CRC doesn't match our CRC. We're gonna send the same chunk again in the next iteration

            continue;

        }

        // If we're using hashing (`hash_algorithm` option) then send our hash of the whole data to the server
        if ( local_hash ) sock.sendd( local_hash.finish() );

    }

    // We came to the end, all chunks are sent successfully
    // We're waiting for servers master bool success flag (if hash algorithm is set server compares our hash with his hash)
    // If this flag is true then we're gonna go home and open a wine
    // If this flag is false then throw exception :( In future this will be not bool but enum with ok or error codes values
    if ( !sock.recvo<bool>() ) throw std::runtime_error( "server denied whole transfer" );

}


void protocol::send_data( const net::IOSocket& sock, const data_t& data, const TransferOptions& options ) {

    send_data( sock, data.data(), data.size(), options );

}