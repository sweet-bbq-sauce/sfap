/*!
  \file
  \brief String implementation.

  \details
  Lightweight owning, null-terminated string container.

  \copyright Copyright (c) 2025 Wiktor Sołtys

  \note License text intentionally omitted from docs. See repository LICENSE.
*/

#include <string_view>

#include <cstddef>
#include <cstring>

#include <sfap/utils/string.hpp>

sfap::String::String(std::string_view source) noexcept : data_(source.size() + 1) {
    if (data_) {
        data_.resize(data_.capacity());
        if (!source.empty())
            std::memcpy(data_.data(), source.data(), source.size());

        data_[source.size()] = std::byte{0};
    }
}

sfap::String::String(const char* source) noexcept : data_(source ? std::strlen(source) + 1 : 0) {
    if (data_) {
        data_.resize(data_.capacity());
        if (size())
            std::memcpy(data_.data(), source, size() + 1);
    }
}

sfap::String::String(const String& s) noexcept : data_(s.data_.size()) {
    if (data_) {
        data_.resize(data_.capacity());
        if (!s.empty())
            std::memcpy(data_.data(), s.data_.data(), size() + 1);
    }
}

sfap::String::operator bool() const noexcept {
    return static_cast<bool>(data_);
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
