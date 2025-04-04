#include <string>
#include <fstream>

#ifdef _WIN32

    #include <ws2tcpip.h>
    
#else

    #include <arpa/inet.h>

#endif

#include <zlib.h>

#include <utils/crc.hpp>
#include <utils/misc.hpp>


using namespace sfap;
using namespace sfap::utils;


crc_t CRC32::calculate( const void* data, qword_t size ) {

    if ( size == 0 ) throw std::logic_error( "data size can't be zero" );
    if ( data == nullptr ) throw std::invalid_argument( "data is null" );

    crc_t crc = 0;
    qword_t converted = 0;

    constexpr qword_t int_max = static_cast<qword_t>( UINT_MAX );

    while ( converted < size ) {

        const z_size_t to_convert = static_cast<z_size_t>( std::min( ( size - converted ), int_max ) );

        crc = static_cast<crc_t>( ::crc32( crc, reinterpret_cast<const Bytef*>( data ) + converted, static_cast<uInt>( to_convert ) ) );

        if ( crc == 0 ) throw std::runtime_error( "crc32() computation  failed" );

        converted += static_cast<qword_t>( to_convert );

    }

    return htonl( crc );

}


crc_t CRC32::calculate( const data_t& data ) {

    return calculate( data.data(), data.size() );

}


crc_t CRC32::calculate( const std::string& data ) {

    return calculate( data.data(), data.size() );

}


crc_t CRC32::file( const path_t& file ) {

    std::ifstream input( file, std::ios::binary );

    if ( !input ) throw std::runtime_error( glue( "can't open file \"", utils::path_to_string( file ), "\"" ) );

    char buffer[1024 * 16];

    crc_t crc = 0;

    while ( input ) {

        input.read( buffer, sizeof( buffer ) );

        crc = static_cast<crc_t>( ::crc32( crc, reinterpret_cast<const Bytef*>( buffer ), static_cast<uInt>( input.gcount() ) ) );

        if ( crc == 0 ) throw std::runtime_error( "crc32() computation failed" );
        
    }

    if ( !input.eof() ) throw std::runtime_error( glue( "error reading file \"", utils::path_to_string( file ), "\"" ) );

    return htonl( crc );

}