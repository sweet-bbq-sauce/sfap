#include <fstream>
#include <mutex>
#include <algorithm>

#include <utils/firewall/firewall.hpp>
#include <sfap.hpp>


using namespace sfap::utils;


void Firewall::load_from_file( const path_t& file, Action action ) {

    std::ifstream input( file );
    if ( !input.is_open() ) throw std::invalid_argument( utils::glue( "Can't open file ", file.string() ) );

    std::unique_lock lock( _mutex );

    std::string line;
    while ( std::getline( input, line ) ) {

        line.erase( line.begin(), std::find_if( line.begin(), line.end(), []( unsigned char ch ) {

            return !std::isspace( ch );

        }));

        if ( line.empty() || line.front() == '#' ) continue;
        else _table[line] = action;

    }

}


void Firewall::whitelist( const path_t& file ) {

    load_from_file( file, Action::ALLOW );

}


void Firewall::blacklist( const path_t& file ) {

    load_from_file( file, Action::DENY );

}