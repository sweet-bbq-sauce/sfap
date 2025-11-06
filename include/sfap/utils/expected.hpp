#pragma once

#if __has_include(<expected>)
#include <expected>
namespace sfap {
template <class T, class E> using expected = std::expected<T, E>;
template <class E> using unexpected = std::unexpected<E>;
} // namespace sfap
#else
#include <tl/expected.hpp>
namespace sfap {
template <class T, class E> using expected = tl::expected<T, E>;
template <class E> using unexpected = tl::unexpected<E>;
} // namespace sfap
#endif