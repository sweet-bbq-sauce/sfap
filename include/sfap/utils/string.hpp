/*!
  \file
  \brief String interface.

  \details
  Lightweight owning, null-terminated string container.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#pragma once

#include <string_view>

#include <cstddef>

#include <sfap/utils/buffer.hpp>

namespace sfap {

/*!
  \brief Lightweight owning, null-terminated string container.

  \details Holds an internal byte buffer with a trailing `'\0'`.
           Invariant: if the object evaluates to true (`operator bool`), then
           `data_.size() == size() + 1` and `c_str()[size()] == '\0'`.
           An empty instance has `size() == 0` and `c_str() == ""`.

  \note This class only deals with raw bytes (ASCII/UTF-8 safe). No encoding normalization.
*/
class String {

  public:
    /*!
      \brief Constructs from a std::string_view.
      \param source Source view. May contain embedded null characters.

      \post `size() == source.size()`, `view() == source`, `c_str()[size()] == '\0'`.
    */
    explicit String(std::string_view source) noexcept;

    /*!
      \brief Constructs from a C-string pointer.
      \param source Pointer to a null-terminated string. May be nullptr.

      \details If `source == nullptr`, creates an empty string (`size() == 0`, `c_str() == ""`).
               Otherwise copies `strlen(source) + 1` bytes including the null terminator.
    */
    explicit String(const char* source) noexcept;

    /*!
      \brief Copy constructor.
      \post Creates an identical copy including the trailing null terminator.
    */
    String(const String&) noexcept;
    ~String() noexcept = default;

    String& operator=(const String&) = delete;

    String(String&&) noexcept = default;
    String& operator=(String&&) noexcept = default;

    /// \brief Checks if the instance owns a valid buffer.
    explicit operator bool() const noexcept;

    /// \return True if `size() == 0`.
    bool empty() const noexcept;

    /*!
      \brief Returns the character count excluding the null terminator.
      \return Number of characters in the string.
    */
    std::size_t size() const noexcept;

    /*!
      \brief Returns a pointer to the internal null-terminated C-string.
      \return Never nullptr. For empty strings returns `""`.
      \warning Pointer remains valid until the object is modified or destroyed.
    */
    const char* c_str() const noexcept;

    /*!
      \brief Returns a string_view to the data excluding the null terminator.
      \return `std::string_view` spanning `[data, data + size())`.
    */
    std::string_view view() const noexcept;

  private:
    Buffer data_; ///< Underlying byte buffer (`size() + 1` bytes, last is `'\0'`).
};

} // namespace sfap