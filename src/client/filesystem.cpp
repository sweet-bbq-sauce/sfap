#include <client/client.hpp>


using namespace sfap;
using namespace sfap::protocol;


path_t Client::pwd() const {

    _negotiate_command( CommandList::PWD );

    return _sock.recvss();

}


path_t Client::cd( const path_t& directory ) const {

    _negotiate_command( CommandList::CD );

    _sock.sends( directory.string() );
    
    const auto result = _sock.recvo<FilesystemResult>();

    if ( result == FilesystemResult::OK ) return _sock.recvss();
    else throw std::runtime_error( "Server rejected cd(): " + std::to_string( static_cast<int>( result ) ) );

}


std::vector<EntryInfo> Client::ls( const path_t& directory, bool deep, bool hidden, bool stat, bool magic ) const {

    _negotiate_command( CommandList::LS );

    _sock.sends( directory.string() );
    _sock.sendo( deep );
    _sock.sendo( hidden );
    _sock.sendo( stat );
    _sock.sendo( magic );

    const auto result = _sock.recvo<FilesystemResult>();

    if ( result != FilesystemResult::OK ) throw std::invalid_argument( "ls() denied: " + std::to_string( static_cast<int>( result ) ) );

    std::vector<EntryInfo> buffer;

    for ( qword_t i = _sock.recvo<qword_t>(); i > 0; i-- ) {

        EntryInfo info;

        info.path = _sock.recvss();

        info.stat = _sock.recvo<bool>() ? std::make_optional( StatInfo() ) : std::nullopt;
        if ( info.stat ) {

            info.stat->size = _sock.recvo<qword_t>();
            info.stat->type = static_cast<std::filesystem::file_type>( _sock.recvo<byte_t>() );

        }

        info.magic = _sock.recvo<bool>() ? std::make_optional( MagicInfo() ) : std::nullopt;
        if ( info.magic ) {

            for ( qword_t i = _sock.recvo<qword_t>(); i > 0; i-- ) info.magic->description.push_back( _sock.recvss() );
            for ( qword_t i = _sock.recvo<qword_t>(); i > 0; i-- ) info.magic->mime.push_back( _sock.recvss() );
            for ( qword_t i = _sock.recvo<qword_t>(); i > 0; i-- ) info.magic->encoding.push_back( _sock.recvss() );

        }

        buffer.push_back( info );

    }

    return buffer;

}


std::filesystem::space_info Client::space( const path_t& directory ) const {

    _negotiate_command( CommandList::SPACE );

    _sock.sends( directory.string() );

    const auto result = _sock.recvo<FilesystemResult>();
    if ( result != FilesystemResult::OK ) throw std::invalid_argument( "SPACE denied: " + std::to_string( static_cast<int>( result ) ) );

    std::filesystem::space_info info;
    info.available = _sock.recvo<qword_t>();
    info.capacity = _sock.recvo<qword_t>();
    info.free = _sock.recvo<qword_t>();

    return info;

}