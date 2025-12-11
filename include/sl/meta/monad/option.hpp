//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/monad/result.hpp"
#include "sl/meta/type/unit.hpp"

namespace sl::meta {

template <typename T>
using option = result<T, meta::unit>;

template <typename TV>
constexpr auto some(TV&& value) {
    return ok(std::forward<TV>(value));
}

constexpr err_t<meta::unit> none{};

} // namespace sl::meta
