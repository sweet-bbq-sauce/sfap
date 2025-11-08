/*!
  \file
  \brief Detecting and storing address kind.

  \details
  Detects the type of a textual network address.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#pragma once

#include <system_error>

#include <cstdint>

#include <sfap/utils/expected.hpp>
#include <sfap/utils/string.hpp>

namespace sfap {
namespace net {

/*!
  \brief Enumeration of detected address types.

  \details Used to distinguish between raw IP addresses, hostnames, and empty input.
*/
enum class AddressKind : std::uint8_t {

    UNKNOWN, ///< Unknown or unrecognized address type.
    EMPTY,   ///< Empty address.

    IP4, ///< IPv4 address.
    IP6, ///< IPv6 address

    HOSTNAME ///< Hostname or fully qualified domain name.
};

/*!
    \brief Detects the type of a textual network address.

    \param address Input string to classify. May be empty.

    \return
    - On success: \c std::expected containing one of \c AddressType::{EMPTY, IP4, IP6, HOSTNAME, UNKNOWN}.
    - On failure: \c std::unexpected with a \c std::error_code describing the OS error returned by the underlying parser
   (e.g., \c inet_pton).

    \remarks
    - IPv4/IPv6 detection uses textual forms. For IPv6, zone IDs or bracketed forms may be rejected depending on
   implementation.
    - Hostname validation follows DNS label rules (labels 1–63 chars, A–Z/a–z/0–9 and \c '-', no leading/trailing dash).
   FQDN length limits apply.
    - Error category:
        - POSIX: \c std::generic_category() with \c errno.
        - Windows: \c std::system_category() with \c WSAGetLastError().

    \note Function is \c noexcept; errors are reported via \c std::error_code.
*/
sfap::expected<AddressKind, std::error_code> detect_address_kind(const String& address) noexcept;

} // namespace net
} // namespace sfap
