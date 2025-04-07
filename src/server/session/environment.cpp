#include <server/session/session.hpp>
#include <utils/misc.hpp>


using namespace sfap;
using namespace sfap::protocol;


bool Session::accessible( const path_t& target ) const {

    path_t relative = std::filesystem::relative( target, root_directory );

    #ifdef _WIN32

        const auto doubledot = L"..";

    #else

        const auto doubledot = "..";

    #endif

    return ( !relative.empty() && relative.native().find( doubledot ) == std::string::npos );

}

path_t Session::serverify( const path_t& path ) const {

    const std::string native = utils::path_to_string( path.native() );
    const std::string begin = native.substr( 0, 2 );

    path_t resolved;

    if ( begin == "./" ) resolved = root_directory / native.substr( 2 );
    else if ( begin == "~/" ) resolved = home_directory / native.substr( 2 );
    else if ( begin.front() == '/' ) resolved = native;
    else resolved = cwd_directory / native;

    if ( accessible( resolved ) ) return resolved.native();
    else throw std::invalid_argument( "path " + resolved.string() + " is not accessible" );

}

path_t Session::userify( const path_t& path ) const {

    if ( !accessible( path ) ) throw std::invalid_argument( "path " + path.string() + " is not accessible" );

    const auto relative = std::filesystem::relative( path, root_directory );

    if ( relative.native()[0] == '.' ) return "/"; 
    else return path_t( "/" / relative ).generic_string();

}