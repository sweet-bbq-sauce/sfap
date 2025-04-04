#pragma once


#include <memory>
#include <string>
#include <vector>

#include <openssl/ssl.h>

#include <sfap.hpp>


namespace sfap {

    namespace crypto {


        using SSL_t = std::unique_ptr<SSL, decltype( &SSL_free )>;

        
        class SSLContext {

            
            public:

                SSLContext();


                SSLContext( const std::string& private_key, const std::string& certificate, const std::vector<std::string>& ca = {} );
                SSLContext( path_t private_key, path_t certificate, const std::vector<path_t>& ca = {} );


                SSLContext( const SSLContext& other ) = delete;
                SSLContext& operator=( const SSLContext& other ) = delete;
                SSLContext( SSLContext&& other ) noexcept;


                void load_private_key( const std::string& private_key );
                void load_private_key_file( path_t private_key );


                void load_certificate( const std::string& certificate );
                void load_certificate_file( path_t certificate );


                void load_ca_certificate( const std::string& ca_certificate );
                void load_ca_certificate_file( path_t ca_certificate );
                void load_ca_certificate_list( const std::vector<std::string>& ca_certificate );
                void load_ca_certificate_file_list( const std::vector<path_t>& ca_certificate );
                void load_ca_certificate_directory( path_t ca_directory );


                void set_verify_mode( bool verify = true );

                bool valid_key_and_cert() const;

                bool valid() const;
                explicit inline operator bool() const;

                SSL_t create_ssl() const;

                static const std::shared_ptr<SSLContext> default_client_context;


            private:

                void check_if_valid() const;

                std::unique_ptr<SSL_CTX, decltype( &SSL_CTX_free )> _ctx;

        };


    }

}