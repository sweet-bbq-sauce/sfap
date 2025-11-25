/*!
  \file
  \brief Error handling interface.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#pragma once

#include <sfap/utils/expected.hpp>

namespace sfap {

struct error_category {
    virtual ~error_category() = default;
    virtual const char* name() const noexcept = 0;
    virtual const char* message(int code) const noexcept = 0;
};

class error_code {
  public:
    error_code(int code, const error_category& category) noexcept;

    /// \brief Get error category name.
    const char* name() const noexcept;

    /// \brief Get error message.
    const char* message() const noexcept;

    /// \brief Get error code.
    int code() const noexcept;

    /// \brief Get error category.
    const error_category& category() const noexcept;

  private:
    int code_;
    const error_category& category_;
};

bool operator==(const error_category&, const error_category&) noexcept;
bool operator==(const error_code&, const error_code&) noexcept;

enum class errc : int { INVALID_ARGUMENT };

/// \brief Return generic error.
unexpected<error_code> generic_error(errc code) noexcept;

/// \brief Return last system error using `errno`.
unexpected<error_code> system_error() noexcept;

/// \brief Return system error.
unexpected<error_code> system_error(int code) noexcept;

/// \brief Return last network error using `errno` on POSIX or `WSAGetLastError()` on Windows.
unexpected<error_code> network_error() noexcept;

/// \brief Return network error.
unexpected<error_code> network_error(int code) noexcept;

} // namespace sfap