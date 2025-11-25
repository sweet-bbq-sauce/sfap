/*!
  \file
  \brief Hostname resolver implementation.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#include <optional>
#include <utility>

#include <cstring>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#endif

#include <sfap/net/resolve.hpp>
#include <sfap/net/types.hpp>
#include <sfap/utils/expected.hpp>
#include <sfap/utils/string.hpp>

sfap::expected<sfap::net::ipx_t, int> sfap::net::resolve(const String& address, ResolveMode mode) noexcept {
    return resolve(address.c_str(), mode);
}

sfap::expected<sfap::net::ipx_t, int> sfap::net::resolve(const char* address, ResolveMode mode) noexcept {
    if (!address)
        return sfap::unexpected<int>(EAI_NONAME);

    addrinfo hints{};
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;

    switch (mode) {
    case ResolveMode::REQUIRE_IPV4:
        hints.ai_family = AF_INET;
        break;
    case ResolveMode::REQUIRE_IPV6:
        hints.ai_family = AF_INET6;
        break;

    case ResolveMode::PREFER_IPV6:
    case ResolveMode::PREFER_IPV4:
        hints.ai_family = AF_UNSPEC;
    }

    addrinfo* result = nullptr;

    if (const int result_code = ::getaddrinfo(address, nullptr, &hints, &result); result_code != 0)
        return sfap::unexpected<int>(result_code);

    std::optional<ip4_t> found_ipv4{};
    std::optional<ip6_t> found_ipv6{};

    for (addrinfo* info = result; info != nullptr; info = info->ai_next) {
        if (info->ai_family == AF_INET && !found_ipv4) {
            found_ipv4.emplace();
            std::memcpy(found_ipv4->data(), &reinterpret_cast<const sockaddr_in*>(info->ai_addr)->sin_addr,
                        sizeof(ip4_t));
        } else if (info->ai_family == AF_INET6 && !found_ipv6) {
            found_ipv6.emplace();
            std::memcpy(found_ipv6->data(), &reinterpret_cast<const sockaddr_in6*>(info->ai_addr)->sin6_addr,
                        sizeof(ip6_t));
        }

        if (found_ipv4 && found_ipv6)
            break;
    }

    ::freeaddrinfo(result);

    switch (mode) {
    case ResolveMode::REQUIRE_IPV4:
        if (!found_ipv4)
            return sfap::unexpected<int>(EAI_NONAME);
        return ipx_t(*found_ipv4);

    case ResolveMode::REQUIRE_IPV6:
        if (!found_ipv6)
            return sfap::unexpected<int>(EAI_NONAME);
        return ipx_t(*found_ipv6);

    case ResolveMode::PREFER_IPV4:
        return found_ipv4 ? ipx_t(*found_ipv4) : ipx_t(*found_ipv6);

    case ResolveMode::PREFER_IPV6:
        return found_ipv6 ? ipx_t(*found_ipv6) : ipx_t(*found_ipv4);
    }

    std::unreachable();
}