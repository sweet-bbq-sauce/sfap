#pragma once


#include <string>
#include <stdexcept>


namespace sfap {

    namespace net {

        
        class DNSException : public std::runtime_error {

            public:

                explicit DNSException( int error );

                int code() const noexcept;

                static std::string get_message( int error );


            private:

                int _code;

        };


    }

}