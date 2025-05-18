//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/traits/concept.hpp"

#include <utility>

namespace sl::meta {

template <PtrLike T>
constexpr auto& deref(T& v) {
    return *v;
}

template <PtrLike T>
constexpr auto&& deref(T&& v) {
    return std::move(*v);
}

} // namespace sl::meta
