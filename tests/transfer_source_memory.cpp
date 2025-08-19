#include <algorithm>
#include <cstring>
#include <vector>

#include <gtest/gtest.h>

#include <crypto/hash/hash.hpp>
#include <protocol/transfer/source/memory.hpp>


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


TEST( TransferSourceMemory, ConstructorThrowsOnZeroChunkSize ) {

    const data_t data = make_pattern( 16 );

    EXPECT_THROW(
        TransferSourceMemory( data.data(), data.size(), static_cast<dword_t>( 0 ) ),
        std::invalid_argument
    );

}


TEST( TransferSourceMemory, ConstructorAllowsEmptyBufferNullptr ) {

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


TEST( TransferSourceMemory, ConstructorThrowsOnNullWithPositiveSize ) {

    EXPECT_THROW(
        TransferSourceMemory( nullptr, 8, static_cast<dword_t>( 1024 ) ),
        std::invalid_argument
    );

}


TEST( TransferSourceMemory, SingleChunkWhenChunkSizeGreaterThanData ) {

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


TEST( TransferSourceMemory, MultipleExactChunks ) {

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


TEST( TransferSourceMemory, LastChunkSmallerThanChunkSize ) {

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


TEST( TransferSourceMemory, SeekgAndBounds ) {

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


TEST( TransferSourceMemory, RemainingEqualsSizeMinusTellg ) {

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


TEST( TransferSourceMemory, ReassembledDataMatchesOriginalByHash ) {

    const std::size_t total = 7777;
    const dword_t chunk = 1024;
    const data_t data = make_pattern(total);

    TransferSourceMemory src( data.data(), data.size(), chunk );
    data_t rebuilt = read_all_chunks( src );

    auto h_orig = sha256_vec( data );
    auto h_rebuild = sha256_vec( rebuilt );
    EXPECT_EQ( h_orig, h_rebuild );

}


TEST( TransferSourceMemory, SeekThenHashSuffix ) {

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


TEST( TransferSourceMemory, ReturnedPointerWithinOriginalBuffer ) {

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


TEST( TransferSourceMemory, ChunkSizeAccessorReturnsCtorValue ) {

    const data_t data = make_pattern( 16 );
    const dword_t chunk = 1024;

    TransferSourceMemory src( data.data(), data.size(), chunk );
    EXPECT_EQ( src.chunk_size(), chunk );

}

TEST( TransferSourceMemory, PeekDoesNotAdvancePosition ) {

    const std::size_t total = 3000;
    const dword_t chunk = 1024;
    const data_t data = make_pattern( total );
    TransferSourceMemory src( data.data(), data.size(), chunk );

    EXPECT_EQ( src.tellg(), 0u );
    EXPECT_FALSE( src.eof() );

    auto [p1, n1] = src.peek_chunk();
    ASSERT_NE( p1, nullptr );
    EXPECT_EQ( n1, chunk );
    EXPECT_EQ( src.tellg(), 0u );
    EXPECT_EQ( src.remaining(), total );

    auto [p2, n2] = src.peek_chunk();
    ASSERT_NE( p2, nullptr );
    EXPECT_EQ( n2, chunk );
    EXPECT_EQ( p2, p1 );
    EXPECT_EQ( src.tellg(), 0u );

    auto [p3, n3] = src.get_chunk();
    ASSERT_NE( p3, nullptr );
    EXPECT_EQ( n3, chunk );
    EXPECT_EQ( p3, p1 );
    EXPECT_EQ( src.tellg(), chunk );
    EXPECT_EQ( src.remaining(), total - chunk );
}

TEST( TransferSourceMemory, PeekThenGetAcrossBoundary ) {

    const std::size_t total = 2500;
    const dword_t chunk = 1024;
    const data_t data = make_pattern( total );
    TransferSourceMemory src( data.data(), data.size(), chunk );

    (void)src.get_chunk();
    EXPECT_EQ( src.tellg(), chunk );

    auto [p2, n2] = src.peek_chunk();
    ASSERT_NE( p2, nullptr );
    EXPECT_EQ( n2, chunk );
    EXPECT_EQ( src.tellg(), chunk );

    auto [g2, gn2] = src.get_chunk();
    EXPECT_EQ( g2, p2 );
    EXPECT_EQ( gn2, n2 );
    EXPECT_EQ( src.tellg(), 2 * chunk );

    auto [p3, n3] = src.peek_chunk();
    ASSERT_NE( p3, nullptr );
    EXPECT_EQ( n3, static_cast<dword_t>( total - 2 * chunk ) );
    EXPECT_EQ( src.tellg(), 2 * chunk );

    auto [g3, gn3] = src.get_chunk();
    EXPECT_EQ( g3, p3 );
    EXPECT_EQ( gn3, n3 );

    EXPECT_TRUE( src.eof() );
    auto [p4, n4] = src.peek_chunk();
    EXPECT_EQ( p4, nullptr );
    EXPECT_EQ( n4, 0u );
    EXPECT_EQ( src.tellg(), total );
    
}

TEST( TransferSourceMemory, RewindResetsToStartAndAllowsReread ) {

    const std::size_t total = 4096 + 123;
    const dword_t chunk = 1024;
    const data_t data = make_pattern( total );
    TransferSourceMemory src( data.data(), data.size(), chunk );

    data_t first_pass = read_all_chunks( src );
    EXPECT_TRUE( src.eof() );
    EXPECT_EQ( first_pass.size(), total );

    src.rewind();
    EXPECT_FALSE( src.eof() );
    EXPECT_EQ( src.tellg(), 0u );
    EXPECT_EQ( src.remaining(), total );

    data_t second_pass = read_all_chunks( src );
    EXPECT_EQ( first_pass, second_pass );

}


TEST( TransferSourceMemory, PeekAtEOFReturnsNullAndKeepsEOF ) {

    const data_t data = make_pattern( 100 );
    TransferSourceMemory src( data.data(), data.size(), static_cast<dword_t>( 256 ) );

    (void)read_all_chunks( src );
    EXPECT_TRUE( src.eof() );
    EXPECT_EQ( src.remaining(), 0u );

    auto [p, n] = src.peek_chunk();
    EXPECT_EQ( p, nullptr );
    EXPECT_EQ( n, 0u );
    EXPECT_TRUE( src.eof() );
    EXPECT_EQ( src.tellg(), src.size() );

}


TEST( TransferSourceMemory, ChunkSizeRespectedOnAllReads ) {

    const std::size_t total = 7000;
    const dword_t chunk = 1024;
    const data_t data = make_pattern( total );
    TransferSourceMemory src( data.data(), data.size(), chunk );

    for (int i = 0; i < 6; ++i) {
        auto [p, n] = src.get_chunk();
        ASSERT_NE( p, nullptr );
        EXPECT_EQ( n, chunk );
    }

    auto [plast, nlast] = src.peek_chunk();
    ASSERT_NE( plast, nullptr );
    EXPECT_EQ( nlast, static_cast<dword_t>( total - 6 * chunk ) );

    auto [glast, gnlast] = src.get_chunk();
    EXPECT_EQ( glast, plast );
    EXPECT_EQ( gnlast, nlast );

    EXPECT_TRUE( src.eof() );

}


TEST( TransferSourceMemory, SeekThenPeekThenGetConsistency ) {

    const std::size_t total = 10'000;
    const dword_t chunk = 4096;
    const data_t data = make_pattern( total );
    TransferSourceMemory src( data.data(), data.size(), chunk );

    const qword_t pos = 1234;
    src.seekg( pos );
    EXPECT_EQ( src.tellg(), pos );

    auto [p1, n1] = src.peek_chunk();
    ASSERT_NE( p1, nullptr );

    EXPECT_EQ( src.tellg(), pos );

    auto [g1, gn1] = src.get_chunk();
    EXPECT_EQ( g1, p1 );
    EXPECT_EQ( gn1, n1 );
    EXPECT_EQ( src.tellg(), pos + gn1 );

}


TEST( TransferSourceMemory, DynamicChunkSizeMidStream ) {

    const std::size_t total = 5000;
    data_t data = make_pattern( total );

    TransferSourceMemory src( data.data(), data.size(), 1024 );

    auto [p1, n1] = src.get_chunk();
    ASSERT_NE( p1, nullptr );
    EXPECT_EQ( n1, 1024u );
    EXPECT_EQ( src.tellg(), 1024u );

    src.set_chunk_size( 256 );
    EXPECT_EQ( src.chunk_size(), 256u );

    auto [p2, n2] = src.get_chunk();
    ASSERT_NE( p2, nullptr );
    EXPECT_EQ( n2, 256u );
    EXPECT_EQ( src.tellg(), 1024u + 256u );

    src.set_chunk_size( 777 );
    std::vector<byte_t> rebuilt;
    rebuilt.insert( rebuilt.end(), static_cast<const byte_t*>( p1 ), static_cast<const byte_t*>( p1 ) + n1 );
    rebuilt.insert( rebuilt.end(), static_cast<const byte_t*>( p2 ), static_cast<const byte_t*>( p2 ) + n2 );

    while ( true ) {

        auto [p, n] = src.get_chunk();
        if (n == 0) break;
        rebuilt.insert( rebuilt.end(), static_cast<const byte_t*>( p ), static_cast<const byte_t*>( p ) + n );

    }

    ASSERT_EQ( rebuilt.size(), total );
    EXPECT_TRUE( std::equal( rebuilt.begin(), rebuilt.end(), data.begin() ) );

}