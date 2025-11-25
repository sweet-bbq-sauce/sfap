/*!
  \file
  \brief SPSC ring buffer implementation.

  \details
  Single-producer single-consumer (SPSC) byte ring buffer with wait-free
  `put()`/`pop()` and zero-copy batched I/O via {prepare,commit}_{write,read}.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#include <algorithm>
#include <atomic>
#include <bit>
#include <new>
#include <span>
#include <utility>

#include <cstddef>

#include <sfap/utils/ringbuffer.hpp>

sfap::RingBuffer::RingBuffer(std::size_t n) noexcept
    : data_((n && std::has_single_bit(n)) ? new (std::nothrow) std::byte[n] : nullptr), capacity_(data_ ? n : 0),
      is_owner_(data_ != nullptr), mask_(capacity_ ? capacity_ - 1 : 0), head_(0), tail_(0), pending_w_(0),
      pending_r_(0) {}

sfap::RingBuffer::RingBuffer(std::span<std::byte> external) noexcept
    : data_(external.empty() || !std::has_single_bit(external.size()) ? nullptr : external.data()),
      capacity_(data_ ? external.size() : 0), is_owner_(false), mask_(capacity_ ? capacity_ - 1 : 0), head_(0),
      tail_(0), pending_w_(0), pending_r_(0) {}

sfap::RingBuffer::RingBuffer(RingBuffer&& other) noexcept
    : data_(std::exchange(other.data_, nullptr)), capacity_(std::exchange(other.capacity_, 0)),
      is_owner_(std::exchange(other.is_owner_, false)), mask_(std::exchange(other.mask_, 0)),
      head_(other.head_.exchange(0, std::memory_order_relaxed)),
      tail_(other.tail_.exchange(0, std::memory_order_relaxed)), pending_w_(std::exchange(other.pending_w_, 0)),
      pending_r_(std::exchange(other.pending_r_, 0)) {}

sfap::RingBuffer::~RingBuffer() noexcept {
    if (is_owner_)
        delete[] data_;
}

sfap::RingBuffer& sfap::RingBuffer::operator=(RingBuffer&& other) noexcept {
    if (this != &other) {
        if (is_owner_)
            delete[] data_;
        data_ = std::exchange(other.data_, nullptr);
        capacity_ = std::exchange(other.capacity_, 0);
        is_owner_ = std::exchange(other.is_owner_, false);
        mask_ = std::exchange(other.mask_, 0);
        head_.store(other.head_.exchange(0, std::memory_order_relaxed), std::memory_order_relaxed);
        tail_.store(other.tail_.exchange(0, std::memory_order_relaxed), std::memory_order_relaxed);
        pending_w_ = std::exchange(other.pending_w_, 0);
        pending_r_ = std::exchange(other.pending_r_, 0);
    }
    return *this;
}

sfap::RingBuffer::operator bool() const noexcept {
    return data_ != nullptr;
}

std::size_t sfap::RingBuffer::capacity() const noexcept {
    return capacity_;
}

std::size_t sfap::RingBuffer::size() const noexcept {
    return head_.load(std::memory_order_acquire) - tail_.load(std::memory_order_acquire);
}

std::size_t sfap::RingBuffer::free() const noexcept {
    return capacity() - size();
}

bool sfap::RingBuffer::empty() const noexcept {
    return size() == 0;
}

bool sfap::RingBuffer::full() const noexcept {
    return size() == capacity_;
}

void sfap::RingBuffer::clean() noexcept {
    head_.store(0, std::memory_order_relaxed);
    tail_.store(0, std::memory_order_relaxed);
    pending_w_ = pending_r_ = 0;
}

std::size_t sfap::RingBuffer::view_size(View<const std::byte> view) noexcept {
    return view.first.size() + view.second.size();
}

sfap::RingBuffer::InputView sfap::RingBuffer::prepare_write(std::size_t n) noexcept {
    if (!data_ || n == 0)
        return {};

    const std::size_t t{tail_.load(std::memory_order_acquire)};
    const std::size_t h{head_.load(std::memory_order_relaxed)};

    const std::size_t used{h - t};
    const std::size_t free{capacity_ - used};
    const std::size_t available{free > pending_w_ ? free - pending_w_ : 0};

    const std::size_t take{std::min(n, available)};
    if (take == 0)
        return {};

    const std::size_t pos{h & mask_};
    const std::size_t first{std::min(take, capacity_ - pos)};
    const std::size_t second{take - first};

    pending_w_ += take;

    return {std::span<std::byte>(data_ + pos, first), std::span<std::byte>(data_, second)};
}

std::size_t sfap::RingBuffer::commit_write(std::size_t n) noexcept {
    if (n == 0)
        return 0;

    const std::size_t can{std::min(n, pending_w_)};
    if (can == 0)
        return 0;

    const std::size_t h{head_.load(std::memory_order_relaxed)};
    const std::size_t new_h{can + h};

    head_.store(new_h, std::memory_order_release);
    pending_w_ -= can;

    return can;
}

bool sfap::RingBuffer::put(std::byte c) noexcept {
    if (!data_)
        return false;

    const std::size_t t{tail_.load(std::memory_order_acquire)};
    const std::size_t h{head_.load(std::memory_order_relaxed)};
    if ((capacity_ - (h - t)) == 0)
        return false;

    data_[h & mask_] = c;
    head_.store(h + 1, std::memory_order_release);

    return true;
}

sfap::RingBuffer::OutputView sfap::RingBuffer::prepare_read(std::size_t n) noexcept {
    if (!data_ || n == 0)
        return {};

    const std::size_t h{head_.load(std::memory_order_acquire)};
    const std::size_t t{tail_.load(std::memory_order_relaxed)};

    const std::size_t s{h - t};
    const std::size_t avail{s > pending_r_ ? s - pending_r_ : 0};

    const std::size_t take{std::min(n, avail)};
    if (take == 0)
        return {};

    const std::size_t pos{t & mask_};
    const std::size_t first{std::min(take, capacity_ - pos)};
    const std::size_t second{take - first};

    pending_r_ += take;

    return {std::span<const std::byte>(data_ + pos, first), std::span<const std::byte>(data_, second)};
}

std::size_t sfap::RingBuffer::commit_read(std::size_t n) noexcept {
    if (n == 0)
        return 0;
    const std::size_t can{std::min(n, pending_r_)};
    if (can == 0)
        return 0;

    const std::size_t t{tail_.load(std::memory_order_relaxed)};
    const std::size_t new_t{t + can};

    tail_.store(new_t, std::memory_order_release);
    pending_r_ -= can;

    return can;
}

bool sfap::RingBuffer::pop(std::byte& c) noexcept {
    if (!data_)
        return false;

    const std::size_t h{head_.load(std::memory_order_acquire)};
    const std::size_t t{tail_.load(std::memory_order_relaxed)};
    if ((h - t) == 0)
        return false;

    c = data_[t & mask_];
    tail_.store(t + 1, std::memory_order_release);

    return true;
}