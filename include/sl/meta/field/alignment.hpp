//
// Created by usatiynyan on 12/17/23.
//

#pragma once

#include "sl/meta/tuple/decay.hpp"
#include "sl/meta/tuple/for_each_type.hpp"
#include "sl/meta/tuple/tie_as_tuple.hpp"

namespace sl::meta {
namespace detail {

template <typename T>
struct get_sizeof {
    constexpr std::size_t operator()() const { return sizeof(T); }
};

} // namespace detail

template <typename T, typename AlignAsT = T>
struct aligned_field {
    static_assert(alignof(AlignAsT) >= sizeof(T));
    alignas(AlignAsT) T value;
};

template <typename AggregateT>
constexpr std::size_t sum_of_field_sizes() {
    using decayed_tied_tuple = decay_tuple_t<tie_as_tuple_t<AggregateT>>;
    constexpr auto sizes_of_fields = for_each_type<detail::get_sizeof, decayed_tied_tuple>();
    return std::apply([](auto... size_of_field) -> std::size_t { return (size_of_field + ... + 0); }, sizes_of_fields);
}

template <typename AggregateT>
constexpr bool is_tightly_packed = sizeof(AggregateT) == sum_of_field_sizes<AggregateT>();

} // namespace sl::meta
