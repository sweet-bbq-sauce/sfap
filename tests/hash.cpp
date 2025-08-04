#include <fstream>

#include <gtest/gtest.h>

#include <sfap.hpp>
#include <crypto/hash/hash.hpp>
#include <utils/to_hex.hpp>


using namespace sfap;
using namespace sfap::crypto;
using namespace sfap::utils;


TEST( Hash, SHA256_KnownVector ) {

    const std::string input = "abc";
    const auto digest = Hash::hash_data( HashAlgorithm::SHA256, input );

    set_hex_format_mode( HexFormatMode::LOWERCASE );
    EXPECT_EQ( to_hex( digest ), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" );

}


TEST( Hash, MD5_KnownVector ) {

    const std::string input = "abc";
    const auto digest = Hash::hash_data( HashAlgorithm::MD5, input );

    set_hex_format_mode( HexFormatMode::LOWERCASE );
    EXPECT_EQ( to_hex( digest ), "900150983cd24fb0d6963f7d28e17f72" );

}


TEST( Hash, ResetSameAlgorithm ) {

    Hash h( HashAlgorithm::SHA1 );

    h.update("test", 4);

    const auto first = h.finish();
    EXPECT_EQ( to_hex( first ), "a94a8fe5ccb19ba61c4c0873d391e987982fbbd3" );

    h.reset();

    h.update("test", 4);

    const auto second = h.finish();
    EXPECT_EQ( to_hex( second ), "a94a8fe5ccb19ba61c4c0873d391e987982fbbd3" );

    EXPECT_EQ( first, second );

}


TEST( Hash, ResetChangeAlgorithm ) {

    Hash h( HashAlgorithm::SHA256 );

    h.update( "x", 1 );

    const auto first = h.finish();
    EXPECT_EQ( to_hex( first ), "2d711642b726b04401627ca9fbac32f5c8530fb1903cc4db02258717921a4881" );

    h.reset( HashAlgorithm::MD5 );

    h.update( "x", 1 );

    const auto second = h.finish();
    EXPECT_EQ( to_hex( second ), "9dd4e461268c8034f5c8564e155c67a6" );

    EXPECT_EQ( second.size(), 16) ;

}


TEST( Hash, HashFile ) {

    const std::string content = "filecontent";
    std::ofstream f( "testfile.bin", std::ios::binary );

    f.write( content.data(), content.size() );
    f.close();

    const auto digest = Hash::hash_file( HashAlgorithm::SHA512, "testfile.bin" );
    EXPECT_EQ( to_hex( digest ), "69824d3c1d3aee59f8e75a0c0e404493549aecf39c6b6645ebfa35d7c3d0c62ee9d58ecb4f48632793cd8fc4460db52f8a1eec565caab9a853515328b6b7a16a" );
    
}
