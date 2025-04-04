#pragma once


#include <string>
#include <optional>

#include <openssl/ssl.h>

#include <sfap.hpp>
#include <net/address/host.hpp>
#include <crypto/context/context.hpp>


namespace sfap {

    namespace net {


        class IOSocket {

            public:

                IOSocket( socket_t fd, std::optional<crypto::SSL_t> ssl = std::nullopt );
                IOSocket( IOSocket&& other ) noexcept;
                IOSocket& operator=( IOSocket&& other ) noexcept;

                IOSocket( const IOSocket& other ) = delete;
                IOSocket& operator=( const IOSocket& other ) = delete;

                ~IOSocket() noexcept;
                void close() noexcept;

                bool is_open() const noexcept;
                bool is_secure() const noexcept;
                explicit operator bool() const noexcept;

                qword_t get_sent_bytes() const noexcept;
                qword_t get_received_bytes() const noexcept;

                socket_t get_socket() const noexcept;

                Host get_local_host() const;
                Host get_remote_host() const;

                template<typename T>
                void sendo( const T& object ) const {

                    static_assert( std::is_trivially_copyable_v<T>, "T must be trivially copyable" );
                    send( &object, sizeof( object ) );

                }

                void send( const void* data, qword_t size ) const;
                void sendb( const void* data, qword_t size ) const;
                void sendd( const data_t& data ) const;
                void sends( const std::string& data ) const;
                void sendsv( const std::vector<std::string>& data ) const;

                template<typename T>
                T recvo() const {

                    static_assert( std::is_trivially_copyable_v<T>, "T must be trivially copyable" );
                    T object;
                    recv( &object, sizeof( object ) );
                    return object;

                }

                void recv( void* data, qword_t size ) const;
                void recvd( data_t& data ) const;
                void recvs( std::string& data ) const;
                std::string recvss() const;
                void recvsv( std::vector<std::string>& data ) const;


            private:

                socket_t _fd;
                std::optional<crypto::SSL_t> _ssl;

                mutable qword_t _sent, _received;

        };


    }

}