/*!
  \file
  \brief Error handling implementation.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#include <cerrno>
#include <cstring>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <sfap/error.hpp>
#include <sfap/utils/expected.hpp>

sfap::error_code::error_code(int code, const sfap::error_category& category) noexcept
    : code_(code), category_(category) {}

const char* sfap::error_code::name() const noexcept {
    return category_.name();
}

const char* sfap::error_code::message() const noexcept {
    return category_.message(code_);
}

int sfap::error_code::code() const noexcept {
    return code_;
}

const sfap::error_category& sfap::error_code::category() const noexcept {
    return category_;
}

bool sfap::operator==(const error_category& left, const error_category& right) noexcept {
    return &left == &right;
}

bool sfap::operator==(const error_code& left, const error_code& right) noexcept {
    return left.code() == right.code() && left.category() == right.category();
}

struct generic_category final : public sfap::error_category {
    const char* name() const noexcept override {
        return "generic";
    }

    const char* message(int code) const noexcept override {
        const sfap::errc err = static_cast<sfap::errc>(code);
        switch (err) {
        case sfap::errc::INVALID_ARGUMENT:
            return "invalid argument";
        }

        return "unknown error";
    }
};

struct system_category final : public sfap::error_category {
    const char* name() const noexcept override {
        return "system";
    }

    const char* message(int code) const noexcept override {
#if defined(_MSC_VER)
        thread_local char buffer[256];
        if (::strerror_s(buffer, sizeof(buffer), code) != 0) {
            return "unknown error";
        }
        return buffer;
#else
        // POSIX / inne: normalne strerror
        return std::strerror(code);
#endif
    }
};

struct network_category final : public sfap::error_category {
    const char* name() const noexcept override {
        return "network";
    }

    const char* message(int code) const noexcept override {
#if defined(_WIN32)
        thread_local char buffer[256];

        DWORD len = ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
                                     static_cast<DWORD>(code), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer,
                                     static_cast<DWORD>(sizeof(buffer)), nullptr);

        if (len == 0)
            return "unknown error";

        return buffer;
#else
        return std::strerror(code);
#endif
    }
};

sfap::unexpected<sfap::error_code> sfap::generic_error(errc code) noexcept {
    static const generic_category category;
    return sfap::unexpected<sfap::error_code>({static_cast<int>(code), category});
}

sfap::unexpected<sfap::error_code> sfap::system_error() noexcept {
    return system_error(errno);
}

sfap::unexpected<sfap::error_code> sfap::system_error(int code) noexcept {
    static const system_category category;
    return sfap::unexpected<sfap::error_code>({code, category});
}

sfap::unexpected<sfap::error_code> sfap::network_error() noexcept {
#if defined(_WIN32)
    return sfap::network_error(::WSAGetLastError());
#else
    return sfap::network_error(errno);
#endif
}

sfap::unexpected<sfap::error_code> sfap::network_error(int code) noexcept {
    static const network_category category;
    return sfap::unexpected<sfap::error_code>({code, category});
}