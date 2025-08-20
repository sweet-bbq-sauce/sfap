#include <algorithm>
#include <filesystem>
#include <fstream>
#include <vector>

#include <gtest/gtest.h>

#include <sfap.hpp>
#include <crypto/hash/hash.hpp>


using namespace sfap;
using sfap::crypto::Hash;
using sfap::crypto::HashAlgorithm;


namespace {

    data_t make_pattern( std::size_t n ) {

        data_t v( n );
        for ( std::size_t i = 0; i < n; ++i ) v[i] = static_cast<byte_t>( i & 0xFFu );
        return v;

    }

    std::filesystem::path write_temp_file( const data_t& data ) {

        const auto dir = std::filesystem::temp_directory_path() / "sfap_hash_stream_tests";
        std::filesystem::create_directories( dir );

        const auto path = dir / ( "file_" + std::to_string( std::rand() ) + ".bin" );

        std::ofstream out( path, std::ios::binary );

        out.write( reinterpret_cast<const char*>( data.data() ), static_cast<std::streamsize>( data.size() ) );

        return path;

    }

    data_t sha256_raw( const void* p, std::size_t n ) {

        return Hash::hash_data( HashAlgorithm::SHA256, p, n );

    }

    data_t sha256_vec( const data_t& v ) {

        return Hash::hash_data( HashAlgorithm::SHA256, v );

    }

}


TEST( HashStreamTest, FullFileSHA256 ) {

    const std::size_t total = 7777;
    const data_t data = make_pattern( total );
    const auto path = write_temp_file( data );

    std::ifstream in( path, std::ios::binary );
    ASSERT_TRUE( in.is_open() );

    auto h_expected = sha256_vec( data );
    auto h_got = Hash::hash_stream( HashAlgorithm::SHA256, in, std::nullopt, 4096 );

    EXPECT_EQ( h_got, h_expected );

    in.close();
    std::filesystem::remove( path );

}


TEST( HashStreamTest, SuffixFromOffset ) {

    const std::size_t total = 12000;
    const qword_t offset = 1234;
    const data_t data = make_pattern( total );
    const auto path = write_temp_file( data );

    std::ifstream in( path, std::ios::binary );
    ASSERT_TRUE( in.is_open() );

    in.seekg( static_cast<std::streamoff>( offset ), std::ios::beg );

    const auto h_expected = sha256_raw( data.data() + offset, total - static_cast<std::size_t>( offset ) );
    const auto h_got = Hash::hash_stream( HashAlgorithm::SHA256, in, std::nullopt, 1024 );

    EXPECT_EQ( h_got, h_expected );

    in.close();
    std::filesystem::remove( path );

}


TEST( HashStreamTest, PrefixLimitedBySize ) {

    const std::size_t total = 50000;
    const qword_t take = 5000;
    const data_t data = make_pattern( total );
    const auto path = write_temp_file( data );

    std::ifstream in( path, std::ios::binary );
    ASSERT_TRUE( in.is_open() );

    const auto h_expected = sha256_raw( data.data(), static_cast<std::size_t>( take ) );
    const auto h_got = Hash::hash_stream( HashAlgorithm::SHA256, in, take, 64 * 1024 );

    EXPECT_EQ( h_got, h_expected );

    in.close();
    std::filesystem::remove(path);
}


TEST( HashStreamTest, WindowFromOffsetWithSize ) {

    const std::size_t total = 20000;
    const qword_t offset = 3210;
    const qword_t win = 7777;
    const data_t data = make_pattern( total );
    const auto path = write_temp_file( data );

    std::ifstream in( path, std::ios::binary );
    ASSERT_TRUE( in.is_open() );

    in.seekg( static_cast<std::streamoff>( offset ), std::ios::beg );

    const auto h_expected = sha256_raw( data.data() + offset, static_cast<std::size_t>( win ) );
    const auto h_got = Hash::hash_stream( HashAlgorithm::SHA256, in, win, 4096 );

    EXPECT_EQ( h_got, h_expected );

    in.close();
    std::filesystem::remove( path );

}


TEST( HashStreamTest, ChunkSizeOneByte ) {

    const std::size_t total = 2048 + 17;
    const data_t data = make_pattern( total );
    const auto path = write_temp_file( data );

    std::ifstream in( path, std::ios::binary );
    ASSERT_TRUE( in.is_open() );

    const auto h_expected = sha256_vec( data );
    const auto h_got = Hash::hash_stream( HashAlgorithm::SHA256, in, std::nullopt, 1 );

    EXPECT_EQ( h_got, h_expected );

    in.close();
    std::filesystem::remove( path );

}


TEST( HashStreamTest, ThrowsWhenSizeTooLarge ) {

    const std::size_t total = 4096;
    const data_t data = make_pattern( total );
    const auto path = write_temp_file( data );

    std::ifstream in( path, std::ios::binary );
    ASSERT_TRUE( in.is_open() );

    const qword_t offset = 1000;
    in.seekg( static_cast<std::streamoff>( offset ), std::ios::beg );

    const qword_t too_much = ( total - offset ) + 1;

    EXPECT_THROW(
        (void)Hash::hash_stream( HashAlgorithm::SHA256, in, too_much, 4096 ),
        std::runtime_error
    );

    in.close();
    std::filesystem::remove( path );
    
}
