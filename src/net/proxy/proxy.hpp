#pragma once


#include <string>
#include <optional>

#include <net/address/host.hpp>


namespace sfap {

    namespace net {


        class Proxy {

            public:

                enum class Method {

                    SOCKS5H

                };


                Proxy( Method method, const Host& proxy, std::optional<Credentials> credentials = std::nullopt );

                void open_tunnel( const Host& target ) const;


            private:

                void open_socks5h_tunnel( const Host& target ) const;

                Method _method;
                Host _proxy;
                std::optional<Credentials> _credentials;

        };


    }

}