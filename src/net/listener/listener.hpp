#pragma once


#include <net/address/address.hpp>
#include <net/iosocket/iosocket.hpp>


namespace sfap {

    namespace net {


        class Listener {

            public:

                Listener( const Address& address );

                void close() noexcept;

                bool is_opened() const noexcept;
                explicit operator bool() const noexcept;

                IOSocket accept() const;


            private:

                socket_t _fd;
                Address _address;


        };


    }

}