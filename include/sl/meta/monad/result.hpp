//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/type/undefined.hpp"

#include <tl/expected.hpp>

#include <system_error>

namespace sl::meta {

template <typename T, typename E>
using result = ::tl::expected<T, E>;

using ok_tag_t = ::tl::in_place_t;
constexpr ok_tag_t ok_tag;

using err_tag_t = ::tl::unexpect_t;
constexpr err_tag_t err_tag;

template <typename TV>
constexpr auto ok(TV&& v) {
    using T = std::decay_t<TV>;
    return result<T, undefined>{ ::tl::in_place, std::forward<TV>(v) };
}

template <typename EV>
constexpr auto err(EV&& e) {
    return ::tl::make_unexpected(std::forward<EV>(e));
}

inline std::error_code errno_code() { return std::make_error_code(std::errc{ errno }); }
inline auto errno_err() { return err(errno_code()); }

} // namespace sl::meta
