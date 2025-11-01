/*!
  \file
  \brief SPSC ring buffer interface.

  \details
  Single-producer single-consumer (SPSC) byte ring buffer with wait-free
  `put()`/`pop()` and zero-copy batched I/O via {prepare,commit}_{write,read}.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#pragma once

#include <atomic>
#include <span>
#include <utility>

#include <cstddef>

namespace sfap {

/*!
  \brief Single-producer single-consumer ring buffer of bytes.

  \invariant `size() + free() == capacity()`.
  \invariant Capacity is a power of two. Indexing uses `mask_`.

  \par Thread-safety
  - Safe for one producer thread and one consumer thread concurrently.
  - Not safe for multiple producers or multiple consumers.
  - No blocking. No dynamic allocation in steady state.

  \par Exceptions
  All functions are `noexcept`. No exceptions are thrown.

  \par Complexity
  - `put()`/`pop()` amortized O(1).
  - `prepare_*`/`commit_*` O(1).
 */
class RingBuffer {

  public:
    /*!
      \brief Construct an owning buffer with capacity `n`.
      \param n Desired capacity in bytes. Must be a power of two.
      \post `capacity() == n` if construction succeeds.
      \warning If `n` is not power-of-two, object becomes empty invalid instance
               and `operator bool()` returns `false`.
     */
    explicit RingBuffer(std::size_t n) noexcept;

    /*!
      \brief Construct a non-owning buffer over external storage.
      \param external Backing memory. Size must be a power of two.
      \post Buffer does not delete the memory in the destructor.
      \warning Caller owns lifetime and alignment of `external`.
     */
    explicit RingBuffer(std::span<std::byte> external) noexcept;

    /// \brief Move-construct, transferring ownership if any.
    RingBuffer(RingBuffer&&) noexcept;

    RingBuffer(const RingBuffer&) = delete;
    ~RingBuffer() noexcept;

    /// \brief Move-assign, releasing previous storage if owned.
    RingBuffer& operator=(RingBuffer&&) noexcept;
    RingBuffer& operator=(const RingBuffer&) = delete;

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
      \brief Reset content to empty state.
      \post `size() == 0`.
      \warning Not safe if producer and consumer run concurrently.
     */
    void clean() noexcept;

    /// \brief Contiguous head/tail view split at wrap point.
    template <class Byte> using View = std::pair<std::span<Byte>, std::span<Byte>>;

    /// \brief Writable slices for producer.
    using InputView = View<std::byte>;

    /// \brief Readable slices for consumer.
    using OutputView = View<const std::byte>;

    /*!
      \brief Reserve up to `n` bytes for zero-copy write.
      \param n Requested bytes.
      \return Two spans that cover the writable region before and after wrap.
              Total available may be less than `n`.
      \pre Called by producer only.
      \post Call `commit_write(k)` with `k <= view_size(return)` to publish.
     */
    InputView prepare_write(std::size_t n) noexcept;

    /*!
      \brief Publish previously prepared bytes.
      \param n Number of bytes actually written.
      \return Bytes committed (clamped to prepared amount).
      \pre `n <= view_size` from the last `prepare_write`.
      \post Advances `head_` by the committed count.
     */
    std::size_t commit_write(std::size_t n) noexcept;

    /*!
      \brief Try to append a single byte.
      \param c Byte to store.
      \return `true` on success, `false` if buffer is full.
      \pre Producer-only.
     */
    bool put(std::byte c) noexcept;

    /*!
      \brief Reserve up to `n` bytes for zero-copy read.
      \param n Requested bytes.
      \return Two spans that cover readable data before and after wrap.
      \pre Consumer-only.
      \post Call `commit_read(k)` with `k <= view_size(return)` to consume.
     */
    OutputView prepare_read(std::size_t n) noexcept;

    /*!
      \brief Consume previously prepared bytes.
      \param n Number of bytes actually consumed.
      \return Bytes committed (clamped to prepared amount).
      \pre `n <= view_size` from the last `prepare_read`.
      \post Advances `tail_` by the committed count.
     */
    std::size_t commit_read(std::size_t n) noexcept;

    /*!
      \brief Try to pop a single byte.
      \param[out] c Destination for the byte.
      \return `true` on success, `false` if buffer is empty.
      \pre Consumer-only.
     */
    bool pop(std::byte& c) noexcept;

    /*!
      \brief Compute total length of a split view.
      \param view Pair of spans.
      \return `view.first.size() + view.second.size()`.
     */
    static std::size_t view_size(View<const std::byte> view) noexcept;

  private:
    std::byte* data_{};      ///< Base pointer.
    std::size_t capacity_{}; ///< Capacity in bytes (power of two).
    bool is_owner_{};        ///< Owns `data_` and frees on destruction.

    std::size_t mask_{};              ///< `capacity_ - 1` for index wrap.
    std::atomic<std::size_t> head_{}; ///< Producer index (next write).
    std::atomic<std::size_t> tail_{}; ///< Consumer index (next read).

    std::size_t pending_w_{}; ///< Bytes prepared for write not committed.
    std::size_t pending_r_{}; ///< Bytes prepared for read not committed.
};

} // namespace sfap