#include <string>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <codecvt>

#include <net/utils.hpp>
#include <utils/misc.hpp>


using namespace sfap::net;


SocketException::SocketException( int error ) :
    _code( error ),
    std::runtime_error( get_message( error ) )
    {}


int SocketException::code() const noexcept {

    return _code;

}


int SocketException::get_last_error() {

    #ifdef _WIN32

        return WSAGetLastError();

    #else

        return errno;

    #endif

}


std::string SocketException::get_message( int error ) {

    #ifdef _WIN32

        LPSTR buffer = nullptr;
        DWORD length = FormatMessageA(

            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            error,
            MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
            ( LPSTR )&buffer,
            0,
            nullptr

        );

        if ( length == 0 || !buffer ) return utils::glue( "Unknown error (code: ", error, ")" );
                        
        std::string result = std::string( buffer, length );

        if ( buffer ) LocalFree( buffer );

        if ( result.empty() ) return "Can't get error message";

        while ( !result.empty() && ( result.back() == '\n' || result.back() == '\r' ) ) result.pop_back();

        if ( result.empty() ) return "Can't get error message";

        return result;

    #else

        char buffer[1024];

        #ifdef _GNU_SOURCE

            return strerror_r( error, buffer, sizeof( buffer ) );

        #else

            if ( strerror_r( error, buffer, sizeof( buffer ) ) == 0 ) return buffer;
            else return utils::glue( "Unknown error (code: ", error, ")" );

        #endif

    #endif

}
