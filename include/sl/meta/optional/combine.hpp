//
// Created by usatiynyan on 1/3/24.
//

#pragma once

#include <tl/optional.hpp>
#include <tuple>

namespace sl::meta {

template <typename... Ts>
tl::optional<std::tuple<Ts...>> combine(tl::optional<Ts>... optionals) {
    if ((optionals.has_value() && ...)) {
        return std::tuple<Ts...>{ std::move(optionals).value()... };
    } else {
        return tl::nullopt;
    }
}

} // namespace sl::meta
