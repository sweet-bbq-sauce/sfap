/*!
  \file
  \brief Hostname resolver interface.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#pragma once

#include <sfap/error.hpp>
#include <sfap/net/types.hpp>
#include <sfap/utils/string.hpp>

namespace sfap::net {

/*!
  \brief Controls how hostname resolution should treat IPv4 and IPv6 addresses.

  The resolver may receive both IPv4 and IPv6 results from the system
  name service (getaddrinfo). This enum specifies which family is required
  or only preferred when selecting the final address.
*/
enum class ResolveMode {
    /*!
      \brief Require an IPv4 address.

      Resolution fails if no IPv4 address is available, even if IPv6
      addresses exist.
    */
    REQUIRE_IPV4,

    /*!
      \brief Prefer an IPv4 address.

      If both IPv4 and IPv6 addresses are available, IPv4 is chosen.
      If only IPv6 is available, IPv6 is returned.
    */
    PREFER_IPV4,

    /*!
      \brief Require an IPv6 address.

      Resolution fails if no IPv6 address is available, even if IPv4
      addresses exist.
    */
    REQUIRE_IPV6,

    /*!
      \brief Prefer an IPv6 address.

      If both IPv4 and IPv6 addresses are available, IPv6 is chosen.
      If only IPv4 is available, IPv4 is returned.
    */
    PREFER_IPV6
};

namespace config {
extern ResolveMode default_resolve_mode;
}

/*!
  \brief Resolves a hostname or textual IP address to an IPv4/IPv6 address.

  This overload accepts an sfap::String and forwards to the `const char*`
  overload. The result type @c ipx_t represents either an IPv4 or IPv6
  address (e.g. a variant-like type).

  The function never throws and uses the provided @ref ResolveMode
  to select the final address from the list returned by the system
  resolver.

  \param address Hostname or textual IP address to resolve.
  \param mode Resolution policy controlling IPv4/IPv6 selection.
  \return An @c sfap::expected containing:
          - @c ipx_t on success,
          - an @c error_code
*/
sfap::result<ipx_t> resolve(const String& address, ResolveMode mode = config::default_resolve_mode) noexcept;
sfap::result<ipx_t> resolve(const char* address, ResolveMode mode = config::default_resolve_mode) noexcept;

} // namespace sfap::net