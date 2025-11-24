/*!
  \file
  \brief Buffer implementation.

  \details
  Simple dynamically allocated, fixed size byte buffer.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#include <algorithm>
#include <new>
#include <optional>
#include <span>
#include <utility>

#include <cstddef>
#include <cstring>

#include <sfap/utils/buffer.hpp>

sfap::Buffer::Buffer(std::size_t capacity) noexcept
    : data_(capacity ? new (std::nothrow) std::byte[capacity] : nullptr), capacity_(data_ ? capacity : 0), owner_(true),
      size_(0) {}

sfap::Buffer::Buffer(std::span<std::byte> external) noexcept
    : data_(external.data() == nullptr || external.empty() ? nullptr : external.data()),
      capacity_(data_ ? external.size() : 0), owner_(false), size_(0) {}

sfap::Buffer::Buffer(Buffer&& o) noexcept
    : data_(std::exchange(o.data_, nullptr)), capacity_(std::exchange(o.capacity_, 0)),
      owner_(std::exchange(o.owner_, false)), size_(std::exchange(o.size_, 0)) {}

sfap::Buffer::~Buffer() noexcept {
    if (owner_)
        delete[] data_;
}

sfap::Buffer& sfap::Buffer::operator=(Buffer&& o) noexcept {
    if (this != &o) {
        if (owner_)
            delete[] data_;

        data_ = std::exchange(o.data_, nullptr);
        capacity_ = std::exchange(o.capacity_, 0);
        owner_ = std::exchange(o.owner_, false);
        size_ = std::exchange(o.size_, 0);
    }
    return *this;
}

sfap::Buffer::operator bool() const noexcept {
    return data_ != nullptr;
}

std::size_t sfap::Buffer::capacity() const noexcept {
    return capacity_;
}

std::size_t sfap::Buffer::size() const noexcept {
    return size_;
}

std::size_t sfap::Buffer::free() const noexcept {
    return data_ ? capacity_ - size_ : 0;
}

bool sfap::Buffer::empty() const noexcept {
    return data_ ? size_ == 0 : true;
}

bool sfap::Buffer::full() const noexcept {
    return data_ ? size_ == capacity_ : false;
}

bool sfap::Buffer::operator==(const Buffer& o) const noexcept {
    if (size_ != o.size_)
        return false;
    if (data_ == o.data_)
        return true;
    if (!data_ || !o.data_)
        return false;

    return std::memcmp(data_, o.data_, size_) == 0;
}

void sfap::Buffer::clean() noexcept {
    size_ = 0;
}

bool sfap::Buffer::resize(std::size_t n) noexcept {
    if (n > capacity_)
        return false;
    size_ = n;
    return true;
}

std::byte* sfap::Buffer::data() noexcept {
    return data_;
}

const std::byte* sfap::Buffer::data() const noexcept {
    return data_;
}

std::byte* sfap::Buffer::begin() noexcept {
    return data_;
}

const std::byte* sfap::Buffer::begin() const noexcept {
    return data_;
}

std::byte* sfap::Buffer::end() noexcept {
    return data_ ? data_ + size_ : nullptr;
}

const std::byte* sfap::Buffer::end() const noexcept {
    return data_ ? data_ + size_ : nullptr;
}

std::span<std::byte> sfap::Buffer::view() noexcept {
    return data_ ? std::span<std::byte>{data_, size_} : std::span<std::byte>{};
}

std::span<const std::byte> sfap::Buffer::view() const noexcept {
    return data_ ? std::span<const std::byte>{data_, size_} : std::span<const std::byte>{};
}

std::byte& sfap::Buffer::operator[](std::size_t i) noexcept {
    return data_[i];
}

const std::byte& sfap::Buffer::operator[](std::size_t i) const noexcept {
    return data_[i];
}

std::optional<std::byte> sfap::Buffer::at(std::size_t i) const noexcept {
    if (!data_)
        return std::nullopt;
    if (i >= size_)
        return std::nullopt;

    return data_[i];
}

std::optional<std::size_t> sfap::Buffer::find(std::byte c, std::size_t from) const noexcept {
    if (!data_)
        return std::nullopt;
    if (from >= size_)
        return std::nullopt;

    const auto position = std::find(begin() + from, end(), c);

    if (position == end())
        return std::nullopt;
    return position - begin();
}

std::optional<std::size_t> sfap::Buffer::find(std::span<const std::byte> pattern, std::size_t from) const noexcept {
    if (!data_)
        return std::nullopt;
    if (pattern.empty())
        return std::nullopt;
    if (size_ < pattern.size())
        return std::nullopt;
    if (from > size_ - pattern.size())
        return std::nullopt;

    const auto position = std::search(begin() + from, end(), pattern.begin(), pattern.end());

    if (position == end())
        return std::nullopt;
    return position - begin();
}

bool sfap::Buffer::assign(std::span<const std::byte> source) noexcept {
    if (!data_)
        return false;
    if (source.empty()) {
        clean();
        return true;
    }
    if (source.size() > capacity_)
        return false;
    std::memcpy(data_, source.data(), source.size());
    return resize(source.size());
}

bool sfap::Buffer::append(std::span<const std::byte> source) noexcept {
    if (!data_)
        return false;
    if (source.empty())
        return true;
    if (size_ + source.size() > capacity_)
        return false;

    const std::size_t old_size = size_;
    if (!resize(size_ + source.size()))
        return false;

    std::memcpy(data_ + old_size, source.data(), source.size());
    return true;
}

bool sfap::Buffer::push_back(std::byte byte) noexcept {
    if (!data_)
        return false;
    if (size_ >= capacity_)
        return false;

    data_[size_++] = byte;
    return true;
}

std::optional<std::span<std::byte>> sfap::Buffer::subview(std::size_t from, std::size_t count) noexcept {
    if (!data_)
        return std::nullopt;
    if (from > size_)
        return std::nullopt;
    if (count > size_ - from)
        return std::nullopt;

    return std::span<std::byte>{data_ + from, count};
}

std::optional<std::span<const std::byte>> sfap::Buffer::subview(std::size_t from, std::size_t count) const noexcept {
    if (!data_)
        return std::nullopt;
    if (from > size_)
        return std::nullopt;

    if (count == 0) {
        count = size_ - from;
    } else if (from + count > size_) {
        return std::nullopt;
    }

    return std::span<const std::byte>{data_ + from, count};
}