#include <chrono>

extern "C" {

    #include <zlib.h>

}

#include <utils/misc.hpp>


using namespace sfap::utils;


namespace sfap {

    namespace utils {


        timestamp_t timestamp( std::optional<timestamp_t> from ) {

            const auto now = std::chrono::system_clock::now();

            const auto ms = static_cast<timestamp_t>( std::chrono::duration_cast<std::chrono::milliseconds>( now.time_since_epoch() ).count() );

            if ( from ) return ms - from.value();
            else return ms;

        }


        std::string strbool( bool value ) noexcept {

            return value ? "true" : "false";

        }


        std::string path_to_string( const path_t& path ) {

            #ifdef _WIN32
                
                int size_needed = WideCharToMultiByte( CP_UTF8, 0, path.c_str(), -1, nullptr, 0, nullptr, nullptr );
                std::string result( size_needed, 0 );
                WideCharToMultiByte( CP_UTF8, 0, path.c_str(), -1, &result[0], size_needed, nullptr, nullptr );
                return result;

            #else

                return path.string();

            #endif

        }


    }

}