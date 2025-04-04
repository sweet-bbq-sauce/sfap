#pragma once


#include <string>

#include <sfap.hpp>
#include <utils/misc.hpp>


namespace sfap {

    namespace utils {


        extern const std::string endline;
        

        std::string fixed_size( qword_t size, bool use_si_units = false );

        std::string format_datetime( timestamp_t time );
        std::string format_process_time( timestamp_t time );

        std::string to_hex( const data_t& data );
        std::string to_hex( const void* data, qword_t size );

        template<typename T>
        std::string to_hex_o( const T& object ) {

            static_assert( std::is_trivially_copyable_v<T>, "T must be trivially copyable" );
            return to_hex( &object, sizeof( object ) );

        }


    }

}