//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/traits/treat_as.hpp"

#include <type_traits>
#include <utility>

namespace sl::meta {

template <treat_as_ptr T>
constexpr auto& deref(typename std::remove_reference_t<T>& v) {
    return *v;
}

template <treat_as_ptr T>
constexpr auto&& deref(typename std::remove_reference_t<T>&& v) {
    return std::move(*v);
}

} // namespace sl::meta
