#pragma once


#include <string_view>


namespace sfap {

    namespace net {


        enum class AddressType {

            EMPTY,
            UNSUPPORTED,

            IPV4,
            IPV6,
            HOSTNAME

        };


        AddressType detect_address_type( std::string_view address ) noexcept;


    }

}