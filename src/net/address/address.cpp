#include <span>
#include <memory>
#include <vector>

#include <net/address/address.hpp>
#include <net/address/host.hpp>
#include <net/proxy/proxy.hpp>
#include <crypto/context/context.hpp>


using namespace sfap::net;
using namespace sfap::crypto;


Address::Address( const Host& target, std::shared_ptr<crypto::SSLContext> ssl_context, std::span<const Proxy> proxy ) noexcept : _target( target ), _ssl_context( std::move( ssl_context ) ), _proxy( proxy.begin(), proxy.end() ) {}

Address::Address( const std::string& target, std::shared_ptr<crypto::SSLContext> ssl_context, std::span<const Proxy> proxy ) noexcept : _target( target ), _ssl_context( std::move( ssl_context ) ), _proxy( proxy.begin(), proxy.end() ) {}


const Host& Address::get_target() const noexcept {

    return _target;

}


const std::shared_ptr<SSLContext>& Address::get_ssl_context() const noexcept {

    return _ssl_context;

}


const std::vector<Proxy>& Address::get_proxy() const noexcept {

    return _proxy;

}