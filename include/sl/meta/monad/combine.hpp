//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/monad/maybe.hpp"

#include <tuple>

namespace sl::meta {

template <typename... Ts>
maybe<std::tuple<Ts...>> combine(maybe<Ts>... optionals) {
    if ((optionals.has_value() && ... && true)) {
        return std::make_tuple(std::move(optionals).value()...);
    } else {
        return null;
    }
}

} // namespace sl::meta
