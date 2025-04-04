#pragma once


#include <stdexcept>
#include <optional>
#include <fstream>

#include <net/iosocket/iosocket.hpp>
#include <crypto/hash.hpp>
#include <utils/crc.hpp>
#include <sfap.hpp>


namespace sfap {

    namespace protocol {


        struct TransferOptions {

            enum class Presets {

                // Fastest BUT INSECURE !!! 
                FASTEST,    // No CRC, no hash. Fastest and low CPU but use only in localhost or verifed LAN.
    
                // Universal
                FAST,       // 10MiB chunk, CRC32, MD5 hash. Use in verifed networks.
                BALANCED,   // 10MiB chunk, CRC32, BLAKE2b. Default.
                
                // Secure but slower
                SECURE,     // 1MiB chunk, CRC32, SHA256. When you want to be sure.
                PARANOID,   // 1MiB chunk, CRC32, SHA512. What this data must be?
    
            };

            TransferOptions( Presets preset = Presets::BALANCED ) noexcept;

            void set_preset( Presets preset );

            crypto::HashAlgorithm hash_algorithm;
            qword_t chunk_size;

        };


        // Send data from memory
        void send_data( const net::IOSocket& sock, const void* data, qword_t size, const TransferOptions& options );
        void send_data( const net::IOSocket& sock, const data_t& data, const TransferOptions& options );

        // Send data from file
        void send_file( const net::IOSocket& sock, const path_t& path, const TransferOptions& options );

        // Receive data to memory
        void receive_data( const net::IOSocket& sock, void* data, qword_t size, const TransferOptions& options );
        void receive_data( const net::IOSocket& sock, data_t& data, const TransferOptions& options );

        // Receive data to file
        void receive_file( const net::IOSocket& sock, const path_t& path );


    }

}