#include <client/remote_file/remote_file.hpp>


using namespace sfap;
using namespace sfap::protocol;


RemoteFile::RemoteFile( const Client& client, dword_t file_id ) :
    _client( client ),
    _file_id( file_id ),
    _opened( true ) {}


RemoteFile::RemoteFile( RemoteFile&& other ) :
    _client( std::move( other._client ) ),
    _file_id( std::exchange( other._file_id, 0 ) ),
    _opened( std::exchange( other._opened, false ) ) {}


RemoteFile& RemoteFile::operator=( RemoteFile&& other ) {

    if ( &other != this ) {

        _client = std::move( other._client );
        _file_id = std::exchange( other._file_id, 0 );
        _opened = std::exchange( other._opened, false );

    }

    return *this;

}


RemoteFile::~RemoteFile() {

    close();

}


void RemoteFile::close() {

    _client._negotiate_command( CommandList::CLOSE );

    _client._sock.sendo( get_id() );

    _opened = false;

}


dword_t RemoteFile::get_id() const {

    return _file_id;

}