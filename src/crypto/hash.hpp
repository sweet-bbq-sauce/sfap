#pragma once


#include <optional>
#include <string>
#include <memory>

#include <openssl/evp.h>

#include <sfap.hpp>


namespace sfap {

    namespace crypto {


        enum class HashAlgorithm : byte_t {

            NONE,

            MD5,

            SHA1,
            SHA256,
            SHA512,

            SHA3_256,
            SHA3_512,

            BLAKE2s,
            BLAKE2b

        };


        class Hash {

            public:

                Hash( HashAlgorithm algorithm );

                void update( const void* data, qword_t size );
                void update( const std::string& data );
                void update( const data_t& data );

                data_t finish();

                explicit operator bool() const noexcept {

                    return _algorithm != HashAlgorithm::NONE;

                }


            private:

                std::unique_ptr<EVP_MD_CTX, decltype( &EVP_MD_CTX_free )> _ctx;
                std::optional<data_t> _hash;
                const HashAlgorithm _algorithm;

        };


        data_t hash_file( HashAlgorithm algorithm, const path_t& file, std::size_t buffer_size = ( 16 * 1024 * 1024 ) );


        data_t hash_data( HashAlgorithm algorithm, const void* data, std::size_t size );
        data_t hash_data( HashAlgorithm algorithm, const std::string& data );
        data_t hash_data( HashAlgorithm algorithm, const data_t& data );


    }

}