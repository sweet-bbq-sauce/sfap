#include <stdexcept>

#include <net/connect/connect.hpp>
#include <net/iosocket/iosocket.hpp>
#include <net/address/address.hpp>
#include <crypto/utils.hpp>
#include <net/utils.hpp>


namespace sfap {

    namespace net {


        IOSocket connect( const Address& address ) {

            if ( !address.get_target().is_connectable() ) throw std::invalid_argument( "address target is not connectable" );

            const auto addr = address.get_target().to_native();
        
            socket_t fd = socket( addr.ss_family, SOCK_STREAM, IPPROTO_TCP );
        
            if ( fd == INVALID_SOCKET ) throw SocketException();
        
            if ( ::connect( fd, reinterpret_cast<const sockaddr*>( &addr ), sizeof( addr ) ) != 0 ) throw SocketException();

            if ( address.get_ssl_context() ) {
        
                crypto::SSL_t ssl = address.get_ssl_context().get()->create_ssl();

                SSL_set_mode( ssl.get(), SSL_MODE_AUTO_RETRY | SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER );
        
                if ( SSL_set_fd( ssl.get(), fd ) != 1 ) {
        
                    closesocket( fd );
        
                    throw crypto::SSLException();
        
                }
        
                SSL_set_connect_state( ssl.get() );
        
                if ( SSL_connect( ssl.get() ) != 1 ) {
        
                    closesocket( fd );
        
                    throw crypto::SSLException();
        
                }
        
                return IOSocket( fd, std::move( ssl ) );
        
            }
        
            else return IOSocket( fd );
        
        }
        
        
        IOSocket connect( const std::string& address ) {
        
            return connect( Address( address ) );
        
        }


    }

}


