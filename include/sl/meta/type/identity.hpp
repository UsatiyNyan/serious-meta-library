//
// Created by usatiynyan.
//

#pragma once

#include <utility>

namespace sl::meta {

struct identity_t {
    template <typename T>
    constexpr auto operator()(T&& x) const { return std::forward<T>(x); }
};

constexpr identity_t identity;

} // namespace sl::meta
