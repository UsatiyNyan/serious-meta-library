//
// Created by usatiynyan.
//

#pragma once

#include <tl/expected.hpp>

namespace sl::meta {

template <typename T, typename E>
using result = ::tl::expected<T, E>;

template <typename EV>
constexpr auto err(EV&& e) {
    return ::tl::make_unexpected(std::forward<EV>(e));
}

template <typename TV>
constexpr auto ok(TV&& value) {
    return std::forward<TV>(value);
}

} // namespace sl::meta
