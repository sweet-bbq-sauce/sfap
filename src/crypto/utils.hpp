#pragma once


#include <stdexcept>
#include <string>

#include <openssl/err.h>


namespace sfap {

    namespace crypto {


        class SSLException : public std::runtime_error {

            public:

                explicit SSLException( unsigned long error = get_last_error() );

                unsigned long code() const noexcept;

                static unsigned long get_last_error();
                static std::string get_message( int error );


            private:

                unsigned long _code;

        };


    }

}