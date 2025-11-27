/*!
  \file
  \brief Network types.

  \details
  Some basic types used in network modules.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#pragma once

#include <array>
#include <variant>

#include <cstdint>

#include <sfap/utils/expected.hpp>
#include <sfap/utils/string.hpp>

namespace sfap::net {

/// \brief Network socket handle.
using socket_t = std::uint32_t;

/// \brief Network port type in host byte order.
using port_t = std::uint16_t;

/// \brief IPv4 address represented as 4 bytes in network byte order.
using ip4_t = std::array<std::uint8_t, 4>;

/// \brief IPv6 address represented as 16 bytes in network byte order.
using ip6_t = std::array<std::uint8_t, 16>;

/*!
  \brief Generic IPv4/IPv6 address holder.
  Stores either an IPv4 or IPv6 address using `std::variant`.
  Provides access to raw byte data and address family information.
*/
class ipx_t {
  public:
    /// \brief Constructs a default IPv4 address (`0.0.0.0`).
    explicit ipx_t() noexcept;

    /// \brief Constructs from an IPv4 address.
    explicit ipx_t(const ip4_t& ip4) noexcept;

    /// \brief Constructs from an IPv6 address.
    explicit ipx_t(const ip6_t& ip6) noexcept;

    /// \brief IP address family type.
    enum class Family { V4, V6 };

    /// \brief Get address family type.
    Family family() const noexcept;

    /// \return `true` if IPv4.
    bool is_4() const noexcept;

    /// \return `true` if IPv6.
    bool is_6() const noexcept;

    /*!
      \brief Returns the IPv4 address.
      \warning Call only if `family() == Family::V4`.
               Undefined behavior (terminate) otherwise.
    */
    const ip4_t& get_4() const noexcept;

    /*!
      \brief Returns the IPv6 address.
      \warning Call only if `family() == Family::V6`.
               Undefined behavior (terminate) otherwise.
    */
    const ip6_t& get_6() const noexcept;

    /// \return Pointer to underlying address bytes.
    const std::uint8_t* data() const noexcept;

    /// \return 4 for IPv4, 16 for IPv6.
    std::size_t size() const noexcept;

    static bool is_any(const ipx_t& ip) noexcept;

  private:
    std::variant<ip4_t, ip6_t> address_; ///< Address storage.
};

} // namespace sfap::net