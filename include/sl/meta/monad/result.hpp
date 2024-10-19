//
// Created by usatiynyan.
//

#pragma once

#include <tl/expected.hpp>

namespace sl::meta {

template <typename T, typename E>
using result = ::tl::expected<T, E>;

namespace detail {

template <typename T>
struct result_value_holder {
    T value;

    template <typename E>
    [[nodiscard]] constexpr operator result<T, E>() && noexcept {
        return result<T, E>{ ::tl::in_place, std::move(value) };
    }
};

} // namespace detail

template <typename EV>
constexpr auto err(EV&& e) {
    return ::tl::make_unexpected(std::forward<EV>(e));
}

template <typename TV>
constexpr auto ok(TV&& value) {
    using T = std::decay_t<TV>;
    return detail::result_value_holder<T>{
        .value = std::forward<TV>(value),
    };
}

} // namespace sl::meta
