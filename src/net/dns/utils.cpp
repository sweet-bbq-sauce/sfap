#include <string>
#include <stdexcept>

#ifdef _WIN32

    #include <winsock2.h>
    #include <ws2tcpip.h>

#else

    #include <netdb.h>

#endif

#include <net/dns/utils.hpp>


using namespace sfap::net;


DNSException::DNSException( int error ) :
    _code( error ),
    std::runtime_error( get_message( error ) ) {}


int DNSException::code() const noexcept {

    return _code;

}


std::string DNSException::get_message( int error ) {

    #ifdef _WIN32

        return gai_strerrorA( error );

    #else

        return gai_strerror( error );

    #endif

}