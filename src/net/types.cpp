/*!
  \file
  \brief Network types implementation.

  \details
  Some basic types used in network modules.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#include <variant>

#include <sfap/net/types.hpp>

sfap::net::ipx_t::ipx_t() noexcept : address_(ip4_t{}) {}

sfap::net::ipx_t::ipx_t(const ip4_t& ip4) noexcept : address_(ip4) {}

sfap::net::ipx_t::ipx_t(const ip6_t& ip6) noexcept : address_(ip6) {}

sfap::net::ipx_t::Family sfap::net::ipx_t::family() const noexcept {
    return std::holds_alternative<ip4_t>(address_) ? Family::V4 : Family::V6;
}

bool sfap::net::ipx_t::is_4() const noexcept {
    return std::holds_alternative<ip4_t>(address_);
}

bool sfap::net::ipx_t::is_6() const noexcept {
    return std::holds_alternative<ip6_t>(address_);
}

const sfap::net::ip4_t& sfap::net::ipx_t::get_4() const noexcept {
    return std::get<ip4_t>(address_);
}

const sfap::net::ip6_t& sfap::net::ipx_t::get_6() const noexcept {
    return std::get<ip6_t>(address_);
}

const std::uint8_t* sfap::net::ipx_t::data() const noexcept {
    return std::visit([](auto const& a) -> const std::uint8_t* { return a.data(); }, address_);
}

std::size_t sfap::net::ipx_t::size() const noexcept {
    return std::visit([](auto const& a) -> std::size_t { return a.size(); }, address_);
}