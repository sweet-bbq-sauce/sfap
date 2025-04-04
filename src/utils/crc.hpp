#pragma once


#include <string>

#include <sfap.hpp>


namespace sfap {

    namespace utils {


        class CRC32 {

            public:

                CRC32() = delete;
                CRC32( const CRC32& ) = delete;
                CRC32& operator=( const CRC32& ) = delete;
                CRC32( CRC32&& ) = delete;
                CRC32& operator=( CRC32&& ) = delete;

                
                static crc_t calculate( const void* data, qword_t size );
                static crc_t calculate( const data_t& data );
                static crc_t calculate( const std::string& data );

                static crc_t file( const path_t& file );

        };


    }

}