/*!
  \file
  \brief Network address container interface.

  \details
  Contains IP address, port and optionally origin string.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#pragma once

#include <optional>
#include <string_view>

#include <sfap/error.hpp>
#include <sfap/net/types.hpp>
#include <sfap/utils/string.hpp>

namespace sfap::net {

/*!
  \brief Address object holding an IP address and port.

  This class stores:
  - a resolved IP address (IPv4 or IPv6) with an associated port,
  - optionally the original hostname string that was used to resolve it.

   The object can be in an "empty" state (no address), which is reflected by
   operator bool(), is_bindable() and is_connectable().

   All operations are noexcept and do not throw exceptions.
*/
class Address {
  public:
    /*!
      \brief Constructs an empty address.

      The resulting object contains no IP or port.
      operator bool(), is_bindable(), is_connectable() all return false.
    */
    Address() noexcept;

    /*!
      \brief Constructs an address from an IP and port.

      \param ip   Resolved IP address.
      \param port Port number.

      The origin hostname is cleared.
    */
    explicit Address(const ipx_t& ip, port_t port) noexcept;

    /*!
      \brief Constructs an address by resolving a hostname, then setting a port.

      \param hostname Hostname to resolve.
      \param port     Port number.

      Resolution errors are ignored; on failure the object remains empty.
      Use from_hostname() directly if error handling is required.
    */
    explicit Address(const String& hostname, port_t port) noexcept;

    /*!
      \brief Constructs an address by resolving a hostname, then setting a port.

      \param hostname Null-terminated hostname string.
      \param port     Port number.

      Resolution errors are ignored; on failure the object remains empty.
      Use from_hostname() for explicit error handling.
    */
    explicit Address(const char* hostname, port_t port) noexcept;

    /*!
      \brief Sets the address from a resolved IP.

      \param ip Resolved IP.

      If an address exists, only the IP is replaced.
      Otherwise, the address is created with port 0.
      Any stored origin hostname is cleared.
    */
    void from_ip(const ipx_t& ip) noexcept;

    /*!
      \brief Resolves a hostname and stores the resulting IP.
      \param hostname Hostname to resolve.
      \return sfap::no_error() on success, error_code on failure.

      On success:
        - IP is updated or created,
        - origin is set if input was classified as a hostname.

      On failure:
        - the internal state is not modified.
    */
    sfap::error_code from_hostname(const String& hostname) noexcept;
    sfap::error_code from_hostname(const char* hostname) noexcept;

    /*!
      \brief Sets the port of the current address.
      \param port Port to assign.

      \post If the address is empty, this call has no effect.
    */
    void set_port(port_t port) noexcept;

    /// \brief Internal representation of an address (IP + port).
    struct InternalAddress {
        ipx_t ip_;
        port_t port_;
    };

    /*!
      \brief Returns the stored address, if present.
      \return Optional InternalAddress.
    */
    const std::optional<InternalAddress>& get_address() const noexcept;

    /*!
      \brief Returns the original hostname, if preserved.
      \return Optional string_view of the hostname.
    */
    std::optional<std::string_view> get_origin() const noexcept;

    /*!
      \brief Checks whether the object contains an IP address.
      \return true if an address is stored, false otherwise.
    */
    explicit operator bool() const noexcept;

    /*!
      \brief Checks if the address can be used for binding.
      \return true if IP exists (port may be 0).
    */
    bool is_bindable() const noexcept;

    /*!
      \brief Checks if the address can be used for connecting.
      \return true if:
        - an IP is stored,
        - the IP is not an "any" address,
        - the port is non-zero.
    */
    bool is_connectable() const noexcept;

    /*!
      \brief Clears the stored address and origin.
      \post After calling this, the object becomes empty.
    */
    void clear() noexcept;

  private:
    std::optional<InternalAddress> address_;
    std::optional<String> origin_;
};

} // namespace sfap::net