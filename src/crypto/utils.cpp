#include <string>
#include <stdexcept>

#include <crypto/utils.hpp>


using namespace sfap::crypto;


SSLException::SSLException( unsigned long error ) :
    _code( error ),
    std::runtime_error( get_message( error ) )
    {}


unsigned long SSLException::code() const noexcept {

    return _code;

}


unsigned long SSLException::get_last_error() {

    return ERR_get_error();

}


std::string SSLException::get_message( int error ) {

    char buffer[1024];
    ERR_error_string_n( error, buffer, sizeof( buffer ) );
    return buffer;

}