#pragma once


#include <string>


namespace sfap {

    namespace net {


        enum class AddressType {

            EMPTY,
            UNSUPPORTED,

            IPV4,
            IPV6,
            HOSTNAME

        };


        AddressType detect_address_type( const std::string& address ) noexcept;


    }

}