/*!
 *  \file
 *  \brief Source file containing Address class definitions.
 *
 *  \copyright Copyright (c) 2025 Wiktor Sołtys
 *
 *  \cond
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
 *  \endcond
 */


#include <optional>
#include <vector>

#include <crypto/context/context.hpp>
#include <net/address/address.hpp>
#include <net/address/host.hpp>
#include <net/proxy/proxy.hpp>


using namespace sfap;
using namespace sfap::net;


Address::Address( const Host& target ) noexcept :
    _target( target )
{}


Address::Address( const Host& target, const std::vector<Proxy>& proxies ) noexcept :
    _target( target ),
    _proxies( proxies )
{}


Address::Address( const Host& target, const crypto::TLSContext& ssl_context ) noexcept :
    _target( target ),
    _ssl_context( ssl_context )
{}


Address::Address( const Host& target, const crypto::TLSContext& ssl_context, const std::vector<Proxy>& proxies ) noexcept :
    _target( target ),
    _ssl_context( ssl_context ),
    _proxies( proxies )
{}


void Address::set_target( const Host& target ) noexcept {

    _target = target;

}


void Address::set_ssl_context( const crypto::TLSContext& ssl_context ) noexcept {

    _ssl_context = ssl_context;

}


void Address::set_proxies( const std::vector<Proxy>& proxies ) noexcept {

    _proxies = proxies;

}


void Address::add_proxy( const Proxy& proxy ) noexcept {

    _proxies.push_back( proxy );

}


const Host& Address::get_target() const noexcept {

    return _target;

}


const std::optional<std::reference_wrapper<const crypto::TLSContext>> Address::get_ssl_context() const noexcept {

    return _ssl_context;

}


const std::vector<Proxy>& Address::get_proxies() const noexcept {

    return _proxies;

}


bool Address::has_ssl_context() const noexcept {

    return _ssl_context.has_value();

}


bool Address::has_proxy() const noexcept {

    return !_proxies.empty();

}