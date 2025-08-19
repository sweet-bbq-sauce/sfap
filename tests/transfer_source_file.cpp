#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>

#include <gtest/gtest.h>

#include <sfap.hpp>
#include <protocol/transfer/source/file.hpp>


using namespace sfap;
using namespace sfap::protocol;


namespace {

    data_t make_pattern( std::size_t n ) {

        data_t v( n );
        for ( std::size_t i = 0; i < n; ++i ) v[i] = static_cast<byte_t>( i & 0xFFu );
        return v;

    }

    std::filesystem::path write_temp_file( const data_t& data ) {

        const auto dir = std::filesystem::temp_directory_path() / "tsf_tests";
        std::filesystem::create_directories( dir );
        const auto path = dir / ( "file_" + std::to_string( std::rand() ) + ".bin" );
        std::ofstream out( path, std::ios::binary );
        
        out.write( reinterpret_cast<const char*>( data.data() ), static_cast<std::streamsize>( data.size() ) );
        out.close();

        return path;

    }

}


TEST( TransferSourceFile, WindowClampAndPeekGetConsistency ) {

    const std::size_t total = 4096 + 800;
    const dword_t chunk = 1024;
    const std::size_t begin = 123;
    const qword_t window = 2000;

    const data_t data = make_pattern( total );
    const auto path = write_temp_file( data );
    std::ifstream f( path, std::ios::binary );
    ASSERT_TRUE( f.is_open() );

    f.seekg( static_cast<std::streamoff>( begin ), std::ios::beg );
    TransferSourceFile src( f, chunk, window );

    ASSERT_EQ( src.size(), window );
    ASSERT_EQ( src.tellg(), 0u );
    ASSERT_EQ( src.remaining(), window );

    auto [p1, n1] = src.peek_chunk();
    ASSERT_NE( p1, nullptr );
    EXPECT_EQ( n1, chunk);
    EXPECT_EQ( src.tellg(), 0u );
    EXPECT_EQ( src.remaining(), window );

    auto [g1, gn1] = src.get_chunk();
    EXPECT_EQ( g1, p1 );
    EXPECT_EQ( gn1, n1 );
    EXPECT_EQ( src.tellg(), n1 );
    EXPECT_EQ( src.remaining(), window - n1 );

    auto [p2, n2] = src.peek_chunk();
    ASSERT_NE( p2, nullptr );
    EXPECT_EQ( n2, static_cast<dword_t>( window - n1 ) );

    auto [g2, gn2] = src.get_chunk();
    EXPECT_EQ( g2, p2 );
    EXPECT_EQ( gn2, n2 );
    EXPECT_EQ( src.tellg(), window );
    EXPECT_EQ( src.remaining(), 0u );
    EXPECT_TRUE( src.eof() );

    auto [p3, n3] = src.peek_chunk();
    EXPECT_EQ( p3, nullptr );
    EXPECT_EQ( n3, 0u );

    auto [g3, gn3] = src.get_chunk();
    EXPECT_EQ( g3, nullptr );
    EXPECT_EQ( gn3, 0u );

    const auto* b1 = static_cast<const byte_t*>( g1 );
    const auto* b2 = static_cast<const byte_t*>( g2 );
    std::vector<byte_t> rebuilt;
    rebuilt.insert( rebuilt.end(), b1, b1 + gn1 );
    rebuilt.insert( rebuilt.end(), b2, b2 + gn2 );

    ASSERT_EQ( rebuilt.size(), static_cast<std::size_t>( window ) );
    EXPECT_TRUE( std::equal( rebuilt.begin(), rebuilt.end(), data.begin() + begin ) );

    f.close();
    std::filesystem::remove( path );

}
