/*!
  \file
  \brief Detecting and storing address kind implementation.

  \details
  Detects the type of a textual network address.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#include <array>
#include <string_view>

#include <cctype>
#include <cstddef>
#include <cstring>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include <sfap/error.hpp>
#include <sfap/net/address_kind.hpp>
#include <sfap/net/types.hpp>
#include <sfap/utils/string.hpp>

namespace {

const auto is_alpha = [](unsigned char c) noexcept -> bool { return std::isalpha(c); };
const auto is_alnum_hyph = [](unsigned char c) noexcept -> bool { return std::isalnum(c) || c == '-'; };

const auto is_fqdn = [](std::string_view s) noexcept -> bool {
    if (s.empty())
        return false;

    const bool trailing_dot = s.back() == '.';
    if ((!trailing_dot && s.size() > 253) || (trailing_dot && s.size() > 254))
        return false;

    std::size_t label_start = 0;
    std::size_t label_len = 0;
    int labels = 0;

    auto check_label = [&](std::size_t start, std::size_t len) -> bool {
        if (len == 0 || len > 63)
            return false;
        if (s[start] == '-' || s[start + len - 1] == '-')
            return false;
        for (std::size_t i = 0; i < len; ++i)
            if (!is_alnum_hyph((unsigned char)s[start + i]))
                return false;
        return true;
    };

    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '.') {
            if (!check_label(label_start, label_len))
                return false;
            ++labels;
            label_start = i + 1;
            label_len = 0;
        } else {
            ++label_len;
        }
    }

    if (trailing_dot)
        return labels >= 1;

    if (!check_label(label_start, label_len))
        return false;
    ++labels;

    if (labels < 2)
        return false;

    std::string_view tld = s.substr(label_start, label_len);
    const bool puny = tld.rfind("xn--", 0) == 0;
    if (puny) {
        if (tld.size() < 5 || tld.size() > 63)
            return false; // "xn--" + 1..59
        for (std::size_t i = 4; i < tld.size(); ++i)
            if (!is_alnum_hyph((unsigned char)tld[i]))
                return false;
    } else {
        if (tld.size() < 2 || tld.size() > 63)
            return false;
        for (unsigned char c : tld)
            if (!is_alpha(c))
                return false;
    }

    return true;
};

} // namespace

sfap::expected<sfap::net::AddressKind, sfap::error_code>
sfap::net::detect_address_kind(const String& address) noexcept {
    return sfap::net::detect_address_kind(address.c_str());
}

sfap::expected<sfap::net::AddressKind, sfap::error_code> sfap::net::detect_address_kind(const char* address) noexcept {

    const std::size_t address_size = std::strlen(address);

    if (address_size == 0)
        return AddressKind::EMPTY;
    if (address_size > 254)
        return AddressKind::UNKNOWN;

    alignas(in6_addr) std::array<std::byte, sizeof(in6_addr)> sink;

    const int ip4_test_result = ::inet_pton(AF_INET, address, sink.data());
    if (ip4_test_result == 1)
        return AddressKind::IP4;
    else if (ip4_test_result == -1)
        return sfap::network_error();

    const int ip6_test_result = ::inet_pton(AF_INET6, address, sink.data());
    if (ip6_test_result == 1)
        return AddressKind::IP6;
    else if (ip6_test_result == -1)
        return sfap::network_error();

    if (is_fqdn({address, address_size}))
        return AddressKind::HOSTNAME;

    return net::AddressKind::UNKNOWN;
}