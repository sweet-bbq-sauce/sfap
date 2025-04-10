#include <client/client.hpp>


using namespace sfap;
using namespace sfap::protocol;


void Client::read( remotefile_t file, char* data, qword_t size ) const {

    _negotiate_command( CommandList::READ );

    _sock.sendo( file );

    const auto result = _sock.recvo<FileIOResult>();
    if ( result != FileIOResult::OK ) throw std::invalid_argument( "server denied READ: " + std::to_string( static_cast<int>( result ) ) );

    _sock.sendo( size );

    // Update file cache
    _remotefile_cache[file].gcount = _sock.recvo<qword_t>();
    _remotefile_cache[file].eof = _sock.recvo<bool>();
    _remotefile_cache[file].good = _sock.recvo<bool>();

    _sock.recv( data, _remotefile_cache[file].gcount );

}


void Client::write( remotefile_t file, const char* data, qword_t size ) const {

    _negotiate_command( CommandList::WRITE );

    _sock.sendo( file );

    const auto result = _sock.recvo<FileIOResult>();
    if ( result != FileIOResult::OK ) throw std::invalid_argument( "server denied WRITE: " + std::to_string( static_cast<int>( result ) ) );

    _sock.sendb( data, size );

    // Update file cache
    _remotefile_cache[file].eof = _sock.recvo<bool>();
    _remotefile_cache[file].good = _sock.recvo<bool>();

}


std::streampos Client::tellg( remotefile_t file ) const {

    _negotiate_command( CommandList::TELLG );

    _sock.sendo( file );

    const auto result = _sock.recvo<FileIOResult>();
    if ( result != FileIOResult::OK ) throw std::invalid_argument( "server denied TELLG: " + std::to_string( static_cast<int>( result ) ) );

    return _sock.recvo<int64_t>();

}


void Client::seekg( remotefile_t file, std::streamoff offset, std::ios::seekdir dir ) const {

    _negotiate_command( CommandList::SEEKG );

    _sock.sendo( file );

    const auto result = _sock.recvo<FileIOResult>();
    if ( result != FileIOResult::OK ) throw std::invalid_argument( "server denied SEEKG: " + std::to_string( static_cast<int>( result ) ) );

    _sock.sendo( true );

    _sock.sendo( static_cast<int64_t>( offset ) );
    _sock.sendo( static_cast<dword_t>( dir ) );

}


void Client::seekg( remotefile_t file, std::streampos position ) const {

    _negotiate_command( CommandList::SEEKG );

    _sock.sendo( file );

    const auto result = _sock.recvo<FileIOResult>();
    if ( result != FileIOResult::OK ) throw std::invalid_argument( "server denied SEEKG: " + std::to_string( static_cast<int>( result ) ) );

    _sock.sendo( false );

    _sock.sendo( static_cast<int64_t>( position ) );

}


std::streampos Client::tellp( remotefile_t file ) const {

    _negotiate_command( CommandList::TELLP );

    _sock.sendo( file );

    const auto result = _sock.recvo<FileIOResult>();
    if ( result != FileIOResult::OK ) throw std::invalid_argument( "server denied TELLP: " + std::to_string( static_cast<int>( result ) ) );

    return _sock.recvo<int64_t>();

}


void Client::seekp( remotefile_t file, std::streamoff offset, std::ios::seekdir dir ) const {

    _negotiate_command( CommandList::SEEKP );

    _sock.sendo( file );

    const auto result = _sock.recvo<FileIOResult>();
    if ( result != FileIOResult::OK ) throw std::invalid_argument( "server denied SEEKP: " + std::to_string( static_cast<int>( result ) ) );

    _sock.sendo( true );

    _sock.sendo( static_cast<int64_t>( offset ) );
    _sock.sendo( static_cast<dword_t>( dir ) );

}


void Client::seekp( remotefile_t file, std::streampos position ) const {

    _negotiate_command( CommandList::SEEKP );

    _sock.sendo( file );

    const auto result = _sock.recvo<FileIOResult>();
    if ( result != FileIOResult::OK ) throw std::invalid_argument( "server denied SEEKP: " + std::to_string( static_cast<int>( result ) ) );

    _sock.sendo( false );

    _sock.sendo( static_cast<int64_t>( position ) );

}