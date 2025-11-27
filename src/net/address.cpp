/*!
  \file
  \brief Network address container implementation.

  \details
  Contains IP address, port and optionally origin string.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#include <optional>
#include <string_view>

#include <sfap/error.hpp>
#include <sfap/net/address.hpp>
#include <sfap/net/address_kind.hpp>
#include <sfap/net/resolve.hpp>
#include <sfap/net/types.hpp>
#include <sfap/utils/string.hpp>

sfap::net::Address::Address() noexcept : address_(std::nullopt), origin_(std::nullopt) {}

sfap::net::Address::Address(const ipx_t& ip, port_t port) noexcept : origin_(std::nullopt) {
    from_ip(ip);
    set_port(port);
}

sfap::net::Address::Address(const String& hostname, port_t port) noexcept {
    from_hostname(hostname);
    set_port(port);
}

sfap::net::Address::Address(const char* hostname, port_t port) noexcept {
    from_hostname(hostname);
    set_port(port);
}

void sfap::net::Address::from_ip(const ipx_t& ip) noexcept {
    origin_.reset();
    if (address_)
        address_->ip_ = ip;
    else
        address_ = {ip, 0};
}

sfap::error_code sfap::net::Address::from_hostname(const String& hostname) noexcept {
    return from_hostname(hostname.c_str());
}

sfap::error_code sfap::net::Address::from_hostname(const char* hostname) noexcept {
    const auto type = detect_address_kind(hostname);

    if (type == AddressKind::EMPTY || type == AddressKind::UNKNOWN)
        return generic_error(errc::INVALID_ARGUMENT).error();

    const auto result = resolve(hostname);

    if (!result)
        return result.error();

    origin_.reset();

    if (address_)
        address_->ip_ = *result;
    else
        address_ = {*result, 0};

    if (type == AddressKind::HOSTNAME)
        origin_ = String(hostname);

    return sfap::no_error();
}

void sfap::net::Address::set_port(port_t port) noexcept {
    if (address_)
        address_->port_ = port;
}

const std::optional<sfap::net::Address::InternalAddress>& sfap::net::Address::get_address() const noexcept {
    return address_;
}

std::optional<std::string_view> sfap::net::Address::get_origin() const noexcept {
    if (!origin_)
        return std::nullopt;
    return origin_->view();
}

sfap::net::Address::operator bool() const noexcept {
    return address_.has_value();
}

bool sfap::net::Address::is_bindable() const noexcept {
    return address_.has_value();
}

bool sfap::net::Address::is_connectable() const noexcept {
    return address_.has_value() && !ipx_t::is_any(address_->ip_) && address_->port_ != 0;
}

void sfap::net::Address::clear() noexcept {
    address_.reset();
    origin_.reset();
}