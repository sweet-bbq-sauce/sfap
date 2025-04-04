#pragma once


#include <string>
#include <stdexcept>


namespace sfap {

    namespace net {


        class SocketException : public std::runtime_error {

            public:

                explicit SocketException( int error = get_last_error() );

                int code() const noexcept;

                static int get_last_error();
                static std::string get_message( int error );


            private:

                int _code;

        };


    }

}