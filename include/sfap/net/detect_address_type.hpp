/*!
  \file
  \brief `detect_address_type` interface.

  \details
  Detects the type of a textual network address.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#pragma once

#include <system_error>

#include <sfap/net/net.hpp>
#include <sfap/utils/expected.hpp>
#include <sfap/utils/string.hpp>

namespace sfap {
namespace net {

/*!
    \brief Detects the type of a textual network address.

    \param address Input string to classify. May be empty.

    \return
    - On success: \c std::expected containing one of \c AddressType::{EMPTY, IP4, IP6, HOSTNAME, UNKNOWN}.
    - On failure: \c std::unexpected with a \c std::error_code describing the OS error returned by the underlying parser (e.g., \c inet_pton).

    \remarks
    - IPv4/IPv6 detection uses textual forms. For IPv6, zone IDs or bracketed forms may be rejected depending on implementation.
    - Hostname validation follows DNS label rules (labels 1–63 chars, A–Z/a–z/0–9 and \c '-', no leading/trailing dash). FQDN length limits apply.
    - Error category:
        - POSIX: \c std::generic_category() with \c errno.
        - Windows: \c std::system_category() with \c WSAGetLastError().

    \note Function is \c noexcept; errors are reported via \c std::error_code.
*/
sfap::expected<AddressType, std::error_code> detect_address_type(const String& address) noexcept;

} // namespace net
} // namespace sfap
