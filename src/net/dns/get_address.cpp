#include <random>
#include <string>
#include <map>
#include <stdexcept>
#include <algorithm>

#include <net/dns/dns.hpp>
#include <net/address/detect.hpp>


using namespace sfap::net;


std::string DNS::get_random_ipv4() const {

    auto list = get_all_ipv4();

    if ( list.empty() ) throw std::runtime_error( "there is no IPv4 addresses" );

    std::shuffle( list.begin(), list.end(), std::random_device{} );
    
    return list.front();

}


std::string DNS::get_random_ipv6() const {

    auto list = get_all_ipv6();

    if ( list.empty() ) throw std::runtime_error( "there is no IPv6 addresses" );

    std::shuffle( list.begin(), list.end(), std::random_device{} );

    return list.front();

}


std::vector<std::string> DNS::get_all_ipv4() const noexcept {

    std::vector<std::string> buffer;

    for ( const auto& pair : _result ) if ( pair.second == AddressType::IPV4 ) buffer.push_back( pair.first );

    return buffer;

}


std::vector<std::string> DNS::get_all_ipv6() const noexcept {

    std::vector<std::string> buffer;

    for ( const auto& pair : _result ) if ( pair.second == AddressType::IPV6 ) buffer.push_back( pair.first );

    return buffer;
    
}


std::vector<std::string> DNS::get_all() const {

    if ( _result.empty() ) throw std::runtime_error( "there is no addresses" );     // This error shouldn't appear

    std::vector<std::string> buffer;

    buffer.reserve( _result.size() );

    for ( const auto& pair : _result ) buffer.push_back( pair.first );

    return buffer;
    
}


std::string DNS::get_auto() const {

    if ( _result.empty() ) throw std::runtime_error( "there is no address" );   // This error shouldn't appear

    switch ( _mode ) {

        case Mode::IPV4:

            for ( const auto& pair : _result ) if ( pair.second == AddressType::IPV4 ) return pair.first;

            throw std::runtime_error( "there is no IPv4 address" );


        case Mode::IPV6:

            for ( const auto& pair : _result ) if ( pair.second == AddressType::IPV6 ) return pair.first;
            
            throw std::runtime_error( "there is no IPv6 address" );


        case Mode::PREFER_IPV6:

            for ( const auto& pair : _result ) if ( pair.second == AddressType::IPV6 ) return pair.first;
            for ( const auto& pair : _result ) if ( pair.second == AddressType::IPV4 ) return pair.first;

    }

    throw std::logic_error( "ey yo wtf" );      // This error shouldn't appear too

}