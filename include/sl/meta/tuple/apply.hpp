//
// Created by usatiynyan on 8/1/23.
//

#pragma once

#include "sl/meta/traits/count_fields.hpp"
#include "sl/meta/tuple/apply_generated.hpp"

namespace sl::meta {

// apply to every field of an aggregate, for tuple use std::apply
template <typename T, typename F>
auto apply(F&& func, T&& value) {
    return detail::apply(
        std::forward<F>(func), std::forward<T>(value), std::integral_constant<std::size_t, count_fields<T>()>{}
    );
}

} // namespace sl::meta
