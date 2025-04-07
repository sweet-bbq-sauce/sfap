#pragma once


#include <string>
#include <string_view>

#ifdef _WIN32

    #include <winsock2.h>

#else

    #include <arpa/inet.h>

#endif

#include <sfap.hpp>


namespace sfap {

    namespace net {


        class Host {


            public:

                Host() noexcept;
                Host( const std::string& hostname, port_t port );
                Host( const std::string& host );
                Host( const sockaddr_storage& native );

                bool operator==( const Host& other ) const noexcept;
                bool operator==( const std::string& hostname ) const noexcept;
                
                Host& operator=( const Host& other ) noexcept;

                bool is_valid() const noexcept;
                bool is_connectable() const noexcept;

                std::string string() const noexcept;

                void set_hostname( const std::string& hostname );
                const std::string& get_hostname() const noexcept;

                void set_port( port_t port ) noexcept;
                port_t get_port() const noexcept;

                sockaddr_storage to_native() const;


            private:

                std::string _hostname;
                port_t _port;


        };


    }

}