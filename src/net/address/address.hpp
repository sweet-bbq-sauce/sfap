#pragma once


#include <memory>
#include <span>
#include <vector>
#include <optional>

#include <net/address/host.hpp>
#include <net/proxy/proxy.hpp>
#include <crypto/context/context.hpp>


namespace sfap {

    namespace net {


        class Address {

            
            public:

                Address( const Host& target, std::shared_ptr<crypto::SSLContext> ssl_context = nullptr, std::span<const Proxy> proxy = {} ) noexcept;
                Address( const std::string& target, std::shared_ptr<crypto::SSLContext> ssl_context = nullptr, std::span<const Proxy> proxy = {} ) noexcept;

                const Host& get_target() const noexcept;
                const std::shared_ptr<crypto::SSLContext>& get_ssl_context() const noexcept;
                const std::vector<Proxy>& get_proxy() const noexcept;


            private:

                Host _target;
                std::shared_ptr<crypto::SSLContext> _ssl_context;
                std::vector<Proxy> _proxy;
                qword_t _connect_timeout;

        };


    }

}