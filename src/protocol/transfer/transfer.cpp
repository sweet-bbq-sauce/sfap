#include <protocol/transfer/transfer.hpp>


using namespace sfap::protocol;


TransferOptions::TransferOptions( Presets preset ) noexcept {

    set_preset( preset );

}


void TransferOptions::set_preset( Presets preset ) {

    constexpr qword_t size1Mib = 1024 * 1024;
    constexpr qword_t size10Mib = 10 * 1024 * 1024;

    switch ( preset ) {

        case Presets::FASTEST:
            hash_algorithm = crypto::HashAlgorithm::NONE;
            chunk_size = 0;
            break;

        case Presets::FAST:
            hash_algorithm = crypto::HashAlgorithm::MD5;
            chunk_size = size10Mib;
            break;

        case Presets::BALANCED:
            hash_algorithm = crypto::HashAlgorithm::BLAKE2b;
            chunk_size = size10Mib;
            break;

        case Presets::SECURE:
            hash_algorithm = crypto::HashAlgorithm::SHA256;
            chunk_size = size1Mib;
            break;

        case Presets::PARANOID:
            hash_algorithm = crypto::HashAlgorithm::SHA512;
            chunk_size = size1Mib;
            break;

        default:
            throw std::invalid_argument( "invalid preset" );

    }

}