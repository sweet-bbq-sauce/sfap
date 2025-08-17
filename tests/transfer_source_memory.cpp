#include <algorithm>
#include <cstring>
#include <vector>

#include <gtest/gtest.h>

#include <crypto/hash/hash.hpp>
#include <protocol/transfer/source.hpp>


using namespace sfap;
using namespace sfap::protocol;

using sfap::crypto::Hash;
using sfap::crypto::HashAlgorithm;


namespace {

    data_t make_pattern( std::size_t n ) {

        data_t v( n );

        for ( std::size_t i = 0; i < n; ++i ) {

            v[i] = static_cast<byte_t>( i & 0xFF );

        }

        return v;

    }

    data_t read_all_chunks( TransferSource& src ) {

        data_t out;
        out.reserve( static_cast<std::size_t>( src.remaining() ) );

        while ( true ) {

            auto [ptr, len] = src.get_chunk();

            if ( len == 0 ) break;

            const auto* b = static_cast<const byte_t*>( ptr );
            out.insert( out.end(), b, b + len );

        }

        return out;
    }

    data_t sha256_raw( const void* p, std::size_t n ) {

        return Hash::hash_data( HashAlgorithm::SHA256, p, n );

    }

    data_t sha256_vec( const data_t& v ) {

        return Hash::hash_data( HashAlgorithm::SHA256, v );

    }

}


TEST( TransferSourceMemoryTest, ConstructorThrowsOnZeroChunkSize ) {

    const data_t data = make_pattern( 16 );

    EXPECT_THROW(
        TransferSourceMemory( data.data(), data.size(), static_cast<dword_t>( 0 ) ),
        std::invalid_argument
    );

}


TEST( TransferSourceMemoryTest, ConstructorAllowsEmptyBufferNullptr ) {

    TransferSourceMemory src( nullptr, 0, static_cast<dword_t>( 1024 ) );

    EXPECT_EQ( src.size(), 0u );
    EXPECT_EQ( src.remaining(), 0u );
    EXPECT_TRUE( src.eof() );
    EXPECT_EQ( src.tellg(), 0u );

    auto [ptr, len] = src.get_chunk();
    EXPECT_EQ( ptr, nullptr );
    EXPECT_EQ( len, 0u );

    EXPECT_EQ( src.tellg(), 0u );

}


TEST( TransferSourceMemoryTest, ConstructorThrowsOnNullWithPositiveSize ) {

    EXPECT_THROW(
        TransferSourceMemory( nullptr, 8, static_cast<dword_t>( 1024 ) ),
        std::invalid_argument
    );

}


TEST( TransferSourceMemoryTest, SingleChunkWhenChunkSizeGreaterThanData ) {

    const data_t data = make_pattern( 100 );
    TransferSourceMemory src( data.data(), data.size(), static_cast<dword_t>( 1024 ) );

    auto [p1, n1] = src.get_chunk();
    ASSERT_NE( p1, nullptr );
    EXPECT_EQ( n1, 100u );

    auto* b1 = static_cast<const byte_t*>( p1 );
    EXPECT_TRUE( std::equal( b1, b1 + n1, data.begin() ) );

    EXPECT_TRUE( src.eof() );
    EXPECT_EQ( src.remaining(), 0u );

    auto [p2, n2] = src.get_chunk();
    EXPECT_EQ( p2, nullptr );
    EXPECT_EQ( n2, 0u );
}


TEST( TransferSourceMemoryTest, MultipleExactChunks ) {

    const std::size_t total = 4096;
    const dword_t chunk = 1024;
    const data_t data = make_pattern( total );

    TransferSourceMemory src( data.data(), data.size(), chunk );
    std::size_t consumed = 0;

    for ( int i = 0; i < 4; ++i ) {

        auto [ptr, len] = src.get_chunk();
        ASSERT_NE( ptr, nullptr );
        EXPECT_EQ( len, chunk );

        const auto* b = static_cast<const byte_t*>( ptr );
        EXPECT_TRUE( std::equal( b, b + len, data.begin() + consumed ) );

        consumed += len;
        EXPECT_EQ( src.tellg(), consumed );
        EXPECT_EQ( src.remaining(), total - consumed );

    }

    EXPECT_TRUE( src.eof() );

    auto [p2, n2] = src.get_chunk();
    EXPECT_EQ( p2, nullptr );
    EXPECT_EQ( n2, 0u );

}


TEST( TransferSourceMemoryTest, LastChunkSmallerThanChunkSize ) {

    const std::size_t total = 2500;
    const dword_t chunk = 1024;
    const data_t data = make_pattern( total );

    TransferSourceMemory src( data.data(), data.size(), chunk );

    auto [p1, n1] = src.get_chunk();
    auto [p2, n2] = src.get_chunk();
    auto [p3, n3] = src.get_chunk();

    EXPECT_EQ( n1, chunk );
    EXPECT_EQ( n2, chunk );
    EXPECT_EQ( n3, static_cast<dword_t>( total - 2 * chunk ) );

    EXPECT_TRUE( src.eof() );

    const auto* b3 = static_cast<const byte_t*>( p3 );
    EXPECT_TRUE( std::equal( b3, b3 + n3, data.begin() + 2 * chunk ) );

}


TEST( TransferSourceMemoryTest, SeekgAndBounds ) {

    const std::size_t total = 3000;
    const dword_t chunk = 1024;
    const data_t data = make_pattern( total );
    TransferSourceMemory src( data.data(), data.size(), chunk );

    src.seekg( 1500 );
    EXPECT_EQ( src.tellg(), 1500u );
    EXPECT_EQ( src.remaining(), total - 1500 );

    src.seekg( total );
    EXPECT_TRUE( src.eof() );
    EXPECT_EQ( src.remaining(), 0u );
    auto [p, n] = src.get_chunk();
    EXPECT_EQ( p, nullptr );
    EXPECT_EQ( n, 0u );

    EXPECT_THROW( src.seekg( total + 1 ), std::out_of_range );

}


TEST( TransferSourceMemoryTest, RemainingEqualsSizeMinusTellg ) {

    const std::size_t total = 10000;
    const dword_t chunk = 4096;
    const data_t data = make_pattern( total );
    TransferSourceMemory src( data.data(), data.size(), chunk );

    EXPECT_EQ( src.remaining(), src.size() - src.tellg() );
    (void)src.get_chunk();
    EXPECT_EQ( src.remaining(), src.size() - src.tellg() );
    (void)src.get_chunk();
    EXPECT_EQ( src.remaining(), src.size() - src.tellg() );
    (void)src.get_chunk();
    EXPECT_EQ( src.remaining(), 0u );

    EXPECT_TRUE( src.eof() );
}


TEST( TransferSourceMemoryTest, ReassembledDataMatchesOriginalByHash ) {

    const std::size_t total = 7777;
    const dword_t chunk = 1024;
    const data_t data = make_pattern(total);

    TransferSourceMemory src( data.data(), data.size(), chunk );
    data_t rebuilt = read_all_chunks( src );

    auto h_orig = sha256_vec( data );
    auto h_rebuild = sha256_vec( rebuilt );
    EXPECT_EQ( h_orig, h_rebuild );

}


TEST( TransferSourceMemoryTest, SeekThenHashSuffix ) {

    const std::size_t total = 5000;
    const dword_t chunk = 1000;
    const data_t data = make_pattern( total );

    const qword_t offset = 1234;
    TransferSourceMemory src( data.data(), data.size(), chunk );
    src.seekg( offset );

    data_t suffix_rebuilt = read_all_chunks( src );

    auto h_suffix_expected = sha256_raw( data.data() + offset, total - static_cast<std::size_t>( offset ) );
    auto h_suffix_got = sha256_vec( suffix_rebuilt );

    EXPECT_EQ( h_suffix_expected, h_suffix_got );

}


TEST( TransferSourceMemoryTest, ReturnedPointerWithinOriginalBuffer ) {

    const std::size_t total = 3500;
    const dword_t chunk = 1024;
    const data_t data = make_pattern( total );

    const auto* base = data.data();
    TransferSourceMemory src( base, data.size(), chunk );

    std::size_t consumed = 0;

    while ( true ) {

        auto [ptr, len] = src.get_chunk();

        if ( len == 0 ) break;
        ASSERT_NE( ptr, nullptr );

        const auto* p = static_cast<const byte_t*>( ptr );
        const auto* start = base;
        const auto* end   = base + total;

        EXPECT_TRUE( p >= start );
        EXPECT_TRUE( p + len <= end );
        EXPECT_TRUE( std::equal( p, p + len, start + consumed ) );

        consumed += len;

    }

    EXPECT_EQ( consumed, total );

}
