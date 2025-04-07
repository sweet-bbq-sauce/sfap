#pragma once


#include <string>
#include <map>
#include <string_view>

#include <net/address/detect.hpp>
#include <net/address/address.hpp>
#include <net/address/host.hpp>


namespace sfap {

    namespace net {

        
        class DNS {


            public:

                enum class Mode {

                    IPV4,
                    IPV6,
                    PREFER_IPV6

                };

                
                #ifndef SFAP_DNS_DEFAULT_RESOLVE_MODE

                    #define SFAP_DNS_DEFAULT_RESOLVE_MODE 1

                #endif

                
                static constexpr Mode default_mode =
                    #if SFAP_DNS_DEFAULT_RESOLVE_MODE == 1
                        Mode::IPV4
                    #elif SFAP_DNS_DEFAULT_RESOLVE_MODE == 2
                        Mode::IPV6
                    #elif SFAP_DNS_DEFAULT_RESOLVE_MODE == 3
                        Mode::PREFER_IPV6
                    #else
                        #error "Invalid SFAP_DNS_DEFAULT_RESOLVE_MODE. Allowed values: 1 (IPv4), 2 (IPv6), 3 (prefer IPv6)"
                    #endif
                ;
                


                explicit DNS( const std::string& hostname, Mode mode = default_mode );
                DNS( const Host& host );
                DNS( const Address& address );

                void set_mode( Mode mode );

                bool has_ipv4() const noexcept;
                bool has_ipv6() const noexcept;

                std::string get_random_ipv4() const;
                std::string get_random_ipv6() const;

                std::vector<std::string> get_all_ipv4() const noexcept;
                std::vector<std::string> get_all_ipv6() const noexcept;

                std::vector<std::string> get_all() const;

                std::string get_auto() const;


            private:

                void _resolve( const std::string& hostname );

                std::map<std::string, AddressType> _result;
                Mode _mode;

                bool _has_ipv4, _has_ipv6;


        };


    }

}