#include <string>
#include <memory>
#include <stdexcept>
#include <fstream>

#include <openssl/evp.h>

#include <sfap.hpp>
#include <crypto/hash.hpp>


namespace sfap {

    namespace crypto {


        Hash::Hash( HashAlgorithm algorithm ) :
            _ctx( EVP_MD_CTX_new(), EVP_MD_CTX_free ),
            _algorithm( algorithm ),
            _hash( std::nullopt ) {

            const EVP_MD* evp;

            switch ( algorithm ) {

                case HashAlgorithm::NONE:
                    evp = nullptr;
                    break;

                case HashAlgorithm::MD5:
                    evp = EVP_md5();
                    break;

                case HashAlgorithm::SHA1:
                    evp = EVP_sha1();
                    break;

                case HashAlgorithm::SHA256:
                    evp = EVP_sha256();
                    break;

                case HashAlgorithm::SHA512:
                    evp = EVP_sha512();
                    break;

                case HashAlgorithm::SHA3_256:
                    evp = EVP_sha3_256();
                    break;

                case HashAlgorithm::SHA3_512:
                    evp = EVP_sha3_512();
                    break;

                case HashAlgorithm::BLAKE2s:
                    evp = EVP_blake2s256();
                    break;

                case HashAlgorithm::BLAKE2b:
                    evp = EVP_blake2b512();
                    break;

                default:
                    throw std::runtime_error( "Unsupported hash algorithm" );

            }

            EVP_DigestInit_ex( _ctx.get(), evp, nullptr );

        }


        void Hash::update( const void* data, qword_t size ) {

            if ( !*this ) throw std::logic_error( "this hash object is not valid" );

            EVP_DigestUpdate( _ctx.get(), data, size );

        }

        void Hash::update( const std::string& data ) {

            update( data.data(), data.size() );

        }

        void Hash::update( const data_t& data ) {

            update( data.data(), data.size() );

        }

        data_t Hash::finish() {

            if ( !_hash ) {

                unsigned char buffer[EVP_MAX_MD_SIZE];
                unsigned int length;

                EVP_DigestFinal_ex( _ctx.get(), buffer, &length );

                _hash = data_t( buffer, buffer + length );

                return _hash.value();

            }

            else return _hash.value();

        }


        data_t hash_file( HashAlgorithm algorithm, const path_t& file, std::size_t buffer_size ) {

            std::ifstream input( file, std::ios::binary );

            if ( !input.is_open() ) throw std::runtime_error( "Can't open " + file.string() );

            char buffer[ buffer_size ];

            Hash hash( algorithm );

            while ( input ) {

                input.read( buffer, buffer_size );

                hash.update( buffer, input.gcount() );

            }

            return hash.finish();

        }


        data_t hash_data( HashAlgorithm algorithm, const void* data, std::size_t size ) {

            Hash hash( algorithm );

            hash.update( data, size );

            return hash.finish();

        }

        data_t hash_data( HashAlgorithm algorithm, const std::string& data ) {

            return hash_data( algorithm, data.data(), data.size() );

        }

        data_t hash_data( HashAlgorithm algorithm, const data_t& data ) {

            return hash_data( algorithm, reinterpret_cast<const char*>( data.data() ), data.size() );

        }


    }

}
