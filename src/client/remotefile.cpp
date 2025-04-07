#include <client/client.hpp>


using namespace sfap;
using namespace sfap::protocol;


remotefile_t Client::open_file( const path_t& file, std::ios::openmode mode ) const {

    _negotiate_command( CommandList::OPEN );

    _sock.sends( file.string() );
    _sock.sendo( static_cast<dword_t>( mode ) );

    const auto result = _sock.recvo<FileIOResult>();
    if ( result != FileIOResult::OK ) throw std::invalid_argument( "server denied OPEN: " + std::to_string( static_cast<int>( result ) ) );

    const remotefile_t id = _sock.recvo<bool>();
    _remotefile_row tmp;

    tmp.eof = _sock.recvo<bool>();
    tmp.good = _sock.recvo<bool>();
    tmp.gcount = 0;

    _remotefile_cache[id] = tmp;

    return id;

}


void Client::close_file( remotefile_t file ) const {

    _negotiate_command( CommandList::CLOSE );

    _sock.sendo( file );

    const auto result = _sock.recvo<FileIOResult>();
    if ( result != FileIOResult::OK ) throw std::invalid_argument( "server denied CLOSE: " + std::to_string( static_cast<int>( result ) ) );

}


bool Client::eof( remotefile_t file ) const {

    _negotiate_command( CommandList::IS_EOF );

    _sock.sendo( file );

    const auto result = _sock.recvo<FileIOResult>();
    if ( result != FileIOResult::OK ) throw std::invalid_argument( "server denied IS_EOF: " + std::to_string( static_cast<int>( result ) ) );

    const auto value = _sock.recvo<bool>();

    _remotefile_cache[file].eof = value;

    return value;

}


bool Client::good( remotefile_t file ) const {

    _negotiate_command( CommandList::GOOD );

    _sock.sendo( file );

    const auto result = _sock.recvo<FileIOResult>();
    if ( result != FileIOResult::OK ) throw std::invalid_argument( "server denied GOOD: " + std::to_string( static_cast<int>( result ) ) );

    const auto value = _sock.recvo<bool>();

    _remotefile_cache[file].good = value;

    return value;

}


qword_t Client::gcount( remotefile_t file ) const {

    _negotiate_command( CommandList::GCOUNT );

    _sock.sendo( file );

    const auto result = _sock.recvo<FileIOResult>();
    if ( result != FileIOResult::OK ) throw std::invalid_argument( "server denied GCOUNT: " + std::to_string( static_cast<int>( result ) ) );

    const auto value = _sock.recvo<qword_t>();

    _remotefile_cache[file].gcount = value;

    return value;

}