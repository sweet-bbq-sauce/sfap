/*!
  \file
  \brief String implementation.

  \details
  Lightweight owning, null-terminated string container.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#include <optional>
#include <span>
#include <string_view>

#include <cstddef>
#include <cstring>

#include <sfap/utils/string.hpp>

sfap::String::String(std::string_view source) noexcept : String(source.size()) {
    if (data_)
        assign(source);
}

sfap::String::String(const char* source) noexcept : data_(source ? std::strlen(source) + 1 : 0) {
    if (source && data_)
        assign(source);
}

sfap::String::String(const String& s) noexcept : String(s.size()) {
    if (data_)
        assign(s.view());
}

sfap::String::String(std::size_t n) noexcept : data_(n + 1) {
    resize(0);
}

sfap::String::operator bool() const noexcept {
    return static_cast<bool>(data_);
}

std::size_t sfap::String::capacity() const noexcept {
    return data_ ? data_.capacity() - 1 : 0;
}

bool sfap::String::empty() const noexcept {
    return size() == 0;
}

std::size_t sfap::String::size() const noexcept {
    return data_ ? data_.size() - 1 : 0;
}

const char* sfap::String::c_str() const noexcept {
    return data_ ? reinterpret_cast<const char*>(data_.data()) : "";
}

std::string_view sfap::String::view() const noexcept {
    return data_ ? std::string_view{c_str(), size()} : std::string_view{};
}

char& sfap::String::operator[](std::size_t i) noexcept {
    return data()[i];
}

const char& sfap::String::operator[](std::size_t i) const noexcept {
    return data()[i];
}

std::optional<char> sfap::String::at(std::size_t i) const noexcept {
    if (!data_ || i >= size())
        return std::nullopt;

    return data()[i];
}

bool sfap::String::resize(std::size_t n) noexcept {
    if (!data_ || n > capacity())
        return false;
    if (!data_.resize(n + 1))
        return false;

    data_[size()] = std::byte{0};

    return true;
}

bool sfap::String::assign(std::string_view source) noexcept {
    if (!data_ || source.size() > capacity())
        return false;
    if (!resize(source.size()))
        return false;

    std::memcpy(data_.data(), source.data(), source.size());

    return true;
}

bool sfap::String::assign(const char* source) noexcept {
    return assign({source, source ? std::strlen(source) : 0});
}

bool sfap::String::append(std::string_view source) noexcept {
    if (!data_ || size() + source.size() > capacity())
        return false;

    const std::size_t old_size = size();
    if (!resize(size() + source.size()))
        return false;

    std::memcpy(data_.data() + old_size, source.data(), source.size());

    return true;
}

bool sfap::String::append(const char* source) noexcept {
    return append({source, source ? std::strlen(source) : 0});
}

bool sfap::String::push_back(char ch) noexcept {
    if (!data_)
        return false;
    if (size() >= capacity())
        return false;

    if (!resize(size() + 1))
        return false;

    data_[size() - 1] = static_cast<std::byte>(ch);

    return true;
}

std::optional<std::string_view> sfap::String::subview(std::size_t from, std::size_t count) const noexcept {
    if (!data_ || from > size())
        return std::nullopt;

    if (count == 0)
        count = size() - from;
    else if (from + count > size())
        return std::nullopt;

    return std::string_view{data() + from, count};
}

void sfap::String::clean() noexcept {
    resize(0);
}

char* sfap::String::data() noexcept {
    return data_ ? reinterpret_cast<char*>(data_.data()) : nullptr;
}

const char* sfap::String::data() const noexcept {
    return data_ ? reinterpret_cast<const char*>(data_.data()) : nullptr;
}

char* sfap::String::begin() noexcept {
    return data();
}

const char* sfap::String::begin() const noexcept {
    return data();
}

char* sfap::String::end() noexcept {
    return data_ ? begin() + size() : nullptr;
}

const char* sfap::String::end() const noexcept {
    return data_ ? begin() + size() : nullptr;
}

std::optional<std::size_t> sfap::String::find(char c, std::size_t from) const noexcept {
    if (!data_)
        return std::nullopt;
    if (from >= size())
        return std::nullopt;

    return data_.find(static_cast<std::byte>(c), from);
}

std::optional<std::size_t> sfap::String::find(std::string_view pattern, std::size_t from) const noexcept {
    if (!data_)
        return std::nullopt;
    if (from + pattern.size() > size())
        return std::nullopt;

    return data_.find(std::span<const std::byte>{(const std::byte*)pattern.data(), pattern.size()}, from);
}