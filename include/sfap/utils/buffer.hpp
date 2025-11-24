/*!
  \file
  \brief Buffer interface.

  \details
  Simple dynamically allocated, fixed size byte buffer.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#pragma once

#include <optional>
#include <span>

#include <cstddef>

namespace sfap {

/*!
  \brief Byte buffer with optional ownership and move-only semantics.
  \details Owns memory when constructed with capacity. Acts as non-owning
           view when constructed from external span. Copying disabled.
           Size is the logical used length, not capacity.
*/
class Buffer {

  public:
    /*!
      \brief Construct owning buffer with given capacity.
      \param capacity Number of bytes to allocate. `0` yields null buffer.
      \note Allocation is nothrow. On failure capacity becomes 0.
    */
    explicit Buffer(std::size_t capacity) noexcept;

    /*!
      \brief Construct non-owning view over external memory.
      \param external Span to external storage. Null or empty yields null buffer.
      \note The caller owns the lifetime of \p external.
    */
    explicit Buffer(std::span<std::byte> external) noexcept;

    /// \brief Move construct. Source becomes empty non-owning buffer.
    Buffer(Buffer&&) noexcept;

    Buffer(const Buffer&) = delete;
    ~Buffer() noexcept;

    /*!
      \brief Move assign. Releases owned memory then takes ownership from source.
      \return `*this`
    */
    Buffer& operator=(Buffer&&) noexcept;

    Buffer& operator=(const Buffer&) = delete;

    /*!
      \brief Validity check.
      \return `true` if the instance is properly initialized.
     */
    explicit operator bool() const noexcept;

    /// \return Fixed capacity in bytes.
    std::size_t capacity() const noexcept;

    /// \return Number of bytes currently stored.
    std::size_t size() const noexcept;

    /// \return Number of free bytes available for writing.
    std::size_t free() const noexcept;

    /// \return `true` if `size() == 0`.
    bool empty() const noexcept;

    /// \return `true` if `free() == 0`.
    bool full() const noexcept;

    /*!
      \brief Content equality by size and byte-wise compare.
      \param other Buffer to compare.
      \return `true` if sizes equal and bytes equal. Fast-path when pointers equal.
    */
    bool operator==(const Buffer&) const noexcept;

    /*!
      \brief Reset content to empty state.
      \post `size() == 0`.
    */
    void clean() noexcept;

    /*!
      \brief Change logical size.
      \param n New size in bytes.
      \return `true` on success. `false` if `n > capacity()`.
      \note Does not initialize or erase bytes.
      \post `size() == n`
    */
    bool resize(std::size_t n) noexcept;

    /// \return data pointer or `nullptr`.
    std::byte* data() noexcept;
    const std::byte* data() const noexcept;

    /// \return `data()` (may be `nullptr`).
    std::byte* begin() noexcept;
    const std::byte* begin() const noexcept;

    /// \return `data() + size()` (may be `nullptr`).
    std::byte* end() noexcept;
    const std::byte* end() const noexcept;

    /// \return span `[data(), data()+size()]` or empty span for null buffer.
    std::span<std::byte> view() noexcept;
    std::span<const std::byte> view() const noexcept;

    /*!
      \brief Unchecked index access.
      \param i Index in `[0..size()]`.
      \return Reference to byte at i.
      \warning No bounds check. Undefined behavior on invalid i or null buffer.
    */
    std::byte& operator[](std::size_t i) noexcept;
    const std::byte& operator[](std::size_t i) const noexcept;

    /*!
      \brief Checked access.
      \param i Index.
      \return Byte at i if `i < size()` and buffer non-null, otherwise `std::nullopt`.
    */
    std::optional<std::byte> at(std::size_t i) const noexcept;

    /*!
      \brief Find first occurrence of a byte.
      \param c Byte to find.
      \param from Start index within used range.
      \return Index on success, `std::nullopt` if not found or invalid input.
    */
    std::optional<std::size_t> find(std::byte c, std::size_t from = 0) const noexcept;

    /*!
      \brief Find first occurrence of a pattern.
      \param pattern Non-empty pattern span.
      \param from Start index within used range.
      \return Index of match start or `std::nullopt` if not found or invalid input.
    */
    std::optional<std::size_t> find(std::span<const std::byte> pattern, std::size_t from = 0) const noexcept;

    /*!
      \brief Replace buffer contents with \p source.
      \param source Bytes to copy into the buffer.
      \return `true` on success, `false` if buffer is null or \p source.size() > capacity().
      \post `size() == source.size()` on success; unchanged on failure.
      \note Copies raw bytes; does not allocate.
      \details If \p source is empty, the buffer is cleared (`size()` becomes `0`).
    */
    bool assign(std::span<const std::byte> source) noexcept;

    /*!
      \brief Append \p source to the end of current contents.
      \param source Bytes to append.
      \return true on success, false if buffer is null or size() + source.size() > capacity().
      \post size() increased by \p source.size() on success; unchanged on failure.
      \note No-op for an empty \p source.
    */
    bool append(std::span<const std::byte> source) noexcept;

    /*!
      \brief Append a single byte.
      \param byte Value to push.
      \return `true` on success, `false` if buffer is null or `full()`.
      \post `size()` increased by 1 on success; unchanged on failure.
    */
    bool push_back(std::byte byte) noexcept;

    /*!
      \brief Create a mutable subview of the stored bytes.
      \param from Start index within `[0, size()]`.
      \param count Number of bytes in the view (default `0`). May be 0 to create an empty view.
      \return A span over `[from, from + count]` on success; `std::nullopt` if buffer is null or the range exceeds
      `size()`.
      \warning The returned span is invalidated by any operation that modifies the buffer’s storage or size.
    */
    std::optional<std::span<std::byte>> subview(std::size_t from, std::size_t count = 0) noexcept;

    /*!
      \brief Create a read-only subview of the stored bytes.
      \param from Start index within `[0, size()]`.
      \param count Number of bytes in the view (default `0`). May be 0 to create an empty view.
      \return A const span over `[from, from + count]` on success; `std::nullopt` if buffer is null or the range exceeds
              `size()`.
    */
    std::optional<std::span<const std::byte>> subview(std::size_t from, std::size_t count = 0) const noexcept;

  private:
    std::byte* data_{};      ///< Base pointer.
    std::size_t capacity_{}; ///< Capacity in bytes (power of two).
    bool owner_{};           ///< Owns `data_` and frees on destruction.

    std::size_t size_{}; ///< Used bytes.
};

} // namespace sfap