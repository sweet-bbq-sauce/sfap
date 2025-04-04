#pragma once


#include <string>
#include <sstream>
#include <optional>

#include <sfap.hpp>


namespace sfap {

    namespace utils {


        template<typename... Args>
        std::string glue( Args&&... args ) {

            std::ostringstream oss;

            ( oss << ... << std::forward<Args>( args ) );
            
            return oss.str();

        }

        timestamp_t timestamp( std::optional<qword_t> from = std::nullopt );

        std::string strbool( bool value ) noexcept;

        std::string path_to_string( const path_t& path );


    }

}