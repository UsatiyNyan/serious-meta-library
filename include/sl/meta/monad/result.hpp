//
// Created by usatiynyan.
//

#pragma once

#include <tl/expected.hpp>

#include <system_error>

namespace sl::meta {

template <typename T, typename E>
using result = ::tl::expected<T, E>;

template <typename EV>
constexpr auto err(EV&& e) {
    return ::tl::make_unexpected(std::forward<EV>(e));
}

inline std::error_code errno_code() { return std::make_error_code(std::errc{ errno }); }
inline auto errno_err() { return err(errno_code()); }

} // namespace sl::meta
