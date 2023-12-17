//
// Created by usatiynyan on 8/1/23.
//

#pragma once

#include "sl/meta/traits/constructible.hpp"

namespace sl::meta {
namespace detail {

template<typename T, std::size_t lo, std::size_t hi>
constexpr std::size_t count_fields() {
    if constexpr (lo == hi) {
        return lo == 0 ? 0 : lo - 1;
    } else {
        constexpr std::size_t mid = lo + (hi - lo) / 2;
        if constexpr (is_any_nargs_constructible_v<T, mid>) {
            return count_fields<T, mid + 1, hi>();
        } else {
            return count_fields<T, lo, mid>();
        }
    }
}

} // namespace detail

// count fields in an aggregate, for tuple use std::tuple_size
template <typename AggregateT>
    requires std::is_aggregate_v<AggregateT>
constexpr std::size_t count_fields() {
    return detail::count_fields<std::decay_t<AggregateT>, 0, (sizeof(AggregateT) / sizeof(std::byte) - 1)>();
}

} // namespace sl::meta
