/*!
 *  \file
 *  \brief Source file containing Proxy class definitions.
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
#include <string>

#include <net/address/host.hpp>
#include <net/iosocket/iosocket.hpp>
#include <net/proxy/proxy.hpp>
#include <utils/credentials.hpp>


using namespace sfap;
using namespace sfap::net;


Proxy::Proxy(
                    
    proxy_type type,
    const Host& host,
    const std::optional<std::reference_wrapper<const utils::Credentials>>& credentials
                
) noexcept :
    _type( type ),
    _host( host ),
    _credentials( credentials )
{}


void Proxy::open(
    
    const Host& target,
    const IOSocket& sock

) const {

    switch ( _type ) {

        case proxy_type::HTTP_CONNECT:
            _open_http_connect( target, sock );
            break;

        case proxy_type::SOCKS5:
            _open_socks5( target, sock );
            break;


        default:
            throw std::logic_error( "unknown proxy type" );

    }

}