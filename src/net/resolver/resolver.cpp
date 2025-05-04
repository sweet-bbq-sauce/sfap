/*!
 *  \file
 *  \brief Implementation file for the Resolver class.
 *
 *  This file contains the method definitions for the Resolver class,
 *  which provides IP address resolution functionality with support
 *  for IPv4, IPv6, and preference-based resolution modes.
 *
 *  \copyright Copyright (c) 2025 Wiktor Sołtys
 *
 *  MIT License
 * 
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */


#include <algorithm>
#include <random>
#include <stdexcept>

#include <net/resolver/resolver.hpp>


using namespace sfap;
using namespace sfap::net;


Resolver::Resolver( const std::string& hostname, Mode mode ) :
    _has_ipv4( false ),
    _has_ipv6( false )
{

    set_mode( mode );

    _resolve( hostname );

}


void Resolver::set_mode( Mode mode ) {

    switch ( mode ) {

        case Mode::IPV4:
        case Mode::IPV6:
        case Mode::PREFER_IPV6:
            _mode = mode;
            break;
            

        default:
            throw std::out_of_range( "invalid mode" );
    }

}


bool Resolver::has_ipv4() const noexcept {

    return _has_ipv4;

}


bool Resolver::has_ipv6() const noexcept {

    return _has_ipv6;

}


std::vector<std::string> Resolver::get_all_ipv4() const noexcept {

    std::vector<std::string> ipv4;

    for ( const auto& address : _result ) {

        if ( address.second == address_type::IPV4 ) ipv4.push_back( address.first );

    }

    return ipv4;

}


std::vector<std::string> Resolver::get_all_ipv6() const noexcept {

    std::vector<std::string> ipv6;

    for ( const auto& address : _result ) {

        if ( address.second == address_type::IPV6 ) ipv6.push_back( address.first );

    }

    return ipv6;

}


std::string Resolver::get_random_ipv4() const {

    if ( !_has_ipv4 ) throw std::runtime_error( "no IPv4 addresses available" );

    auto ipv4 = get_all_ipv4();

    std::shuffle( ipv4.begin(), ipv4.end(), std::mt19937{ std::random_device{}() } );

    return ipv4.front();

}


std::string Resolver::get_random_ipv6() const {

    if ( !_has_ipv6 ) throw std::runtime_error( "no IPv6 addresses available" );

    auto ipv6 = get_all_ipv6();

    std::shuffle( ipv6.begin(), ipv6.end(), std::mt19937{ std::random_device{}() } );

    return ipv6.front();

}


std::vector<std::string> Resolver::get_all() const noexcept {

    std::vector<std::string> all;

    for ( const auto& address : _result ) all.push_back( address.first );

    return all;

}


std::string Resolver::get_auto() const {

    if ( _mode == Mode::IPV4 ) {
        
        return get_random_ipv4();
    }
    else if ( _mode == Mode::IPV6 ) {
        
        return get_random_ipv6();

    }
    else {

        if ( has_ipv6() ) return get_random_ipv6();
        else return get_random_ipv4();

    }

}