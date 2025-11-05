/*!
  \file
  \brief Network types.

  \details
  Some basic types used in network modules.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#pragma once

#include <cstdint>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

namespace sfap {
namespace net {

/*!
    \brief Platform-independent socket handle type.

    \details On Windows, this is an alias for \c SOCKET (WinSock type).
             On POSIX systems, this is an alias for \c int (file descriptor).
             Use this type instead of the native one in cross-platform code.

    \note On Windows, you must call \c WSAStartup before using any sockets
          and link against \c ws2_32.
          Ensure that \c <winsock2.h> is included before \c <windows.h>.
*/
#if defined(_WIN32)
using socket_t = SOCKET;
#else
using socket_t = int;
#endif

/*!
    \brief Sentinel value representing an invalid or uninitialized socket.

    \details On Windows: \c INVALID_SOCKET.
             On POSIX: \c -1.

    \see socket_t
*/
#if defined(_WIN32)
constexpr inline socket_t invalid_socket = INVALID_SOCKET;
#else
constexpr inline socket_t invalid_socket = -1;
#endif

/// \brief Network port type in host byte order.
using port_t = uint16_t;

/*!
    \brief Enumeration of detected address types.

    \details Used to distinguish between raw IP addresses, hostnames, and empty input.
*/
enum class AddressType : std::uint8_t {

    UNKNOWN, ///< Unknown or unrecognized address type.
    EMPTY,   ///< Empty address.

    IP4, ///< IPv4 address.
    IP6, ///< IPv6 address

    HOSTNAME ///< Hostname or fully qualified domain name.

};

} // namespace net
} // namespace sfap