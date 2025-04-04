#pragma once


#include <stdexcept>

#include <net/iosocket/iosocket.hpp>
#include <net/address/address.hpp>


namespace sfap {

    namespace net {


        IOSocket connect( const Address& address );

        IOSocket connect( const std::string& address );


    }

}