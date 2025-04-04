#include <string>
#include <vector>
#include <chrono>
#include <cstring>
#include <locale>
#include <sstream>

#include <utils/format.hpp>


namespace sfap {

    namespace utils {


        #ifdef _WIN32

            const std::string endline = "\r\n";

        #else

            const std::string endline = "\n";

        #endif


        std::string fixed_size( qword_t size, bool use_si_units ) {

            if ( size == 0 ) return "0 B";
        
            const static std::vector<std::string> si_units = { "B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };
            const static std::vector<std::string> iec_units = { "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB" };
        
            const long double base = use_si_units ? 1000.0L : 1024.0L;
            const auto& units = use_si_units ? si_units : iec_units;
        
            long double size_double = static_cast<long double>( size );

            if ( size_double < base ) return glue( size, " B" );

            std::size_t unit_index = 0;
        
            while ( size_double >= base && unit_index < units.size() ) {
        
                size_double /= base;
                unit_index++;
        
            }

            return glue( std::setprecision( 2 ), size_double, " ", units[ unit_index ] );
        
        }


        std::string format_datetime( timestamp_t time ) {

            const std::time_t seconds = static_cast<std::time_t>( time / 1000 );
            std::tm time_info = {};

            #ifdef _WIN32

                localtime_s( &time_info, &seconds );

            #else

                localtime_r( &seconds, &time_info );
                
            #endif

            char buffer[256] = {};

            std::strftime( buffer, sizeof( buffer ), "%d %b %Y %H:%M:%S", &time_info );

            std::ostringstream oss;

            oss << buffer << ":" << std::setw( 3 ) << std::setfill( '0' ) << ( time % 1000 );

            return oss.str();

        }


        std::string format_process_time( timestamp_t time ) {

            const timestamp_t elapsed = time - start_time;

            const timestamp_t days = elapsed / ( 1000 * 60 * 60 * 24 );
            const timestamp_t hours = ( elapsed % ( 1000 * 60 * 60 * 24 ) ) / ( 1000 * 60 * 60 );
            const timestamp_t minutes = ( elapsed % ( 1000 * 60 * 60 ) ) / ( 1000 * 60 );
            const timestamp_t seconds = ( elapsed % ( 1000 * 60 ) ) / 1000;
            const timestamp_t milliseconds = elapsed % 1000;

            std::ostringstream oss;

            if ( days > 0 ) oss << "Days: " << days << " ";
            oss << std::setw( 2 ) << std::setfill( '0' ) << hours << ":";
            oss << std::setw( 2 ) << std::setfill( '0' ) << minutes << ":";
            oss << std::setw( 2 ) << std::setfill( '0' ) << seconds << ":";
            oss << std::setw( 3 ) << std::setfill( '0' ) << milliseconds;

            return oss.str();

        }


        std::string to_hex( const data_t& data ) {

            return to_hex( data.data(), data.size() );

        }


        std::string to_hex( const void* data, qword_t size ) {

            if ( !data ) throw std::invalid_argument( "data is null" );
            if ( size == 0 ) return "";

            const byte_t* bytes = static_cast<const uint8_t*>( data );
            std::ostringstream oss;
            oss << std::hex << std::setfill( '0' );

            for ( qword_t i = 0; i < size; ++i ) oss << std::setw( 2 ) << static_cast<int>( bytes[i] );

            return oss.str();

        }
        

    }

}