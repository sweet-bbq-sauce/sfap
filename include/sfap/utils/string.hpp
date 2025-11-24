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
      \brief Constructs from a `std::string_view`.
      \param source Source view. May contain embedded null characters.

      \post `size() == source.size()`, `view() == source`, `c_str()[size()] == '\0'`.
    */
    explicit String(std::string_view source) noexcept;

    /*!
      \brief Constructs from a C-string pointer.
      \param source Pointer to a null-terminated string. May be `nullptr`.

      \details If `source == nullptr`, creates an empty string (`size() == 0`, `c_str() == ""`).
               Otherwise copies `strlen(source) + 1` bytes including the null terminator.
    */
    explicit String(const char* source) noexcept;

    /*!
      \brief Copy constructor.
      \post Creates an identical copy including the trailing null terminator.
    */
    String(const String&) noexcept;

    /*!
      \brief Constructs from a C-string pointer.
      \param source Pointer to a null-terminated string. May be `nullptr`.

      \details If `source == nullptr`, creates an empty string (`size() == 0`, `c_str() == ""`).
               Otherwise copies `strlen(source) + 1` bytes including the null terminator.
    */
    explicit String(std::size_t n) noexcept;

    ~String() noexcept = default;

    String& operator=(const String&) = delete;

    String(String&&) noexcept = default;
    String& operator=(String&&) noexcept = default;

    /// \brief Checks if the instance owns a valid buffer.
    explicit operator bool() const noexcept;

    /// \return String capacity (NUL not included).
    std::size_t capacity() const noexcept;

    /// \return True if `size() == 0`.
    bool empty() const noexcept;

    void clean() noexcept;

    /// \return Number of characters excluding the NUL terminator.
    std::size_t size() const noexcept;

    /*!
      \brief Returns a pointer to the internal null-terminated C-string.
      \return Never nullptr. For empty strings returns `""`.
      \warning Pointer remains valid until the object is modified or destroyed.
    */
    const char* c_str() const noexcept;

    /// \return data pointer or `nullptr`.
    char* data() noexcept;
    const char* data() const noexcept;

    /*!
      \brief Returns a string_view to the data excluding the null terminator.
      \return `std::string_view` spanning `[data, data + size())`.
    */
    std::string_view view() const noexcept;

    /// \return `data()` (may be `nullptr`).
    char* begin() noexcept;
    const char* begin() const noexcept;

    /// \return `data() + size()` (may be `nullptr`).
    char* end() noexcept;
    const char* end() const noexcept;

    /*!
      \brief Unchecked index access.
      \param i Index in [0..size()).
      \return Reference to byte at i.
      \warning No bounds check. Undefined behavior on invalid i or null buffer.
    */
    char& operator[](std::size_t i) noexcept;
    const char& operator[](std::size_t i) const noexcept;

    /*!
      \brief Checked access.
      \param i Index.
      \return Char at i if `i < size()` and buffer non-null, otherwise `std::nullopt`.
    */
    std::optional<char> at(std::size_t i) const noexcept;

    /*!
      \brief Find first occurrence of a character.
      \param c Char to find.
      \param from Start index within used range.
      \return Index on success, `std::nullopt` if not found or invalid input.
    */
    std::optional<std::size_t> find(char c, std::size_t from = 0) const noexcept;

    /*!
      \brief Find first occurrence of a pattern.
      \param pattern Non-empty pattern `std::string_view`.
      \param from Start index within used range.
      \return Index of match start or `std::nullopt` if not found or invalid input.
    */
    std::optional<std::size_t> find(std::string_view pattern, std::size_t from = 0) const noexcept;

    /*!
      \brief Change logical size.
      \param n New size in bytes.
      \return `true` on success. false if `n > capacity()`.
      \note Does not initialize or erase bytes.
      \post `size() == n`
    */
    bool resize(std::size_t n) noexcept;

    /*!
      \brief Replace buffer contents with \p source.
      \param source Bytes to copy into the buffer.
      \return `true` on success, `false` if buffer is null or \p source.size() > capacity().
      \post `size() == source.size()` on success; unchanged on failure.
      \note Copies raw bytes; does not allocate.
      \details If \p source is empty, the buffer is cleared (`size()` becomes `0`).
    */
    bool assign(std::string_view source) noexcept;
    bool assign(const char* source) noexcept;

    /*!
      \brief Append \p source to the end of current contents.
      \param source Bytes to append.
      \return `true` on success, `false` if buffer is null or `size() + source.size() > capacity()`.
      \post `size()` increased by \p source.size() on success; unchanged on failure.
      \note No-op for an empty \p source.
    */
    bool append(std::string_view source) noexcept;
    bool append(const char* source) noexcept;

    /*!
      \brief Append a single character.
      \param byte Value to push.
      \return `true` on success, `false` if buffer is null or `full()`.
      \post `size()` increased by 1 on success; unchanged on failure.
    */
    bool push_back(char ch) noexcept;

    /*!
      \brief Create a read-only subview of the stored bytes.
      \param from Start index within [0, size()].
      \param count Number of bytes in the view (default 0). May be 0 to create an empty view.
      \return A const span over [from, from + count] on success; `std::nullopt` if buffer is null or the range exceeds
      size().
    */
    std::optional<std::string_view> subview(std::size_t from, std::size_t count = 0) const noexcept;

  private:
    Buffer data_; ///< Underlying byte buffer (`size() + 1` bytes, last is `'\0'`).
};

} // namespace sfap