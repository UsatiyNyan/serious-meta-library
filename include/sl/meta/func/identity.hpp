//
// Created by usatiynyan.
//

#pragma once

#include <utility>

namespace sl::meta {

template <typename T>
auto identity(T&& x) {
    return std::forward<T>(x);
}

} // namespace sl::meta
