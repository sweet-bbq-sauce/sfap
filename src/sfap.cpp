#include <locale>

#ifndef _WIN32

    #include <sys/signal.h>

#endif

#include <openssl/ssl.h>

#include <sfap.hpp>
#include <utils/misc.hpp>
#include <utils/format.hpp>
#include <utils/log.hpp>


namespace sfap {


    const qword_t start_time = utils::timestamp();


    bool Credentials::operator==( const Credentials& other ) const {

        return ( other.user == user ) && ( other.password == password );

    }


    std::string StatInfo::get_fixed_size() const {

        return utils::fixed_size( size );

    }


    qword_t StatInfo::get_size() const {

        return size;

    }
    
    
    path_t StatInfo::get_path() const {

        return path;

    }
    
    
    std::filesystem::file_type StatInfo::get_type() const {

        return type;

    }


    MagicInfo::MagicInfo( const std::string& description, const std::string& mime, const std::string& encoding ) :
        _description( description ),
        _mime( mime ),
        _encoding( encoding ) {}


    std::string MagicInfo::get_description() const {

        return _description;

    }


    std::string MagicInfo::get_mime() const {

        return _mime;

    }


    std::string MagicInfo::get_encoding() const {

        return _encoding;

    }


    void init( bool winsock, bool openssl ) {

        //std::setlocale( LC_TIME, "" );

        #ifdef _WIN32

            HANDLE hOut = GetStdHandle( STD_OUTPUT_HANDLE );
            DWORD dwMode = 0;
            
            if ( hOut == INVALID_HANDLE_VALUE || !GetConsoleMode( hOut, &dwMode ) ) {

                utils::warning( "Can't enable color in terminal." );

                return;

            }
            
            SetConsoleMode( hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING );

            WSADATA wsaData;
            if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 ) throw std::runtime_error( "Can't init WinSock2" );

        #else

            signal( SIGPIPE, SIG_IGN );

        #endif

    }


}