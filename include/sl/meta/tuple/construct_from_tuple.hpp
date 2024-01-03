//
// Created by usatiynyan on 1/3/24.
//

#pragma once

#include <tuple>

namespace sl::meta {
namespace detail {

template <typename T, typename TupleT, std::size_t... idxs>
T construct_from_tuple(TupleT& tuple, std::index_sequence<idxs...>) {
    return T{ (std::get<idxs>(tuple))... };
}

} // namespace detail

template <typename T, typename... Ts>
T construct_from_tuple(std::tuple<Ts...>& tuple) {
    return detail::construct_from_tuple<T>(tuple, std::make_index_sequence<sizeof...(Ts)>());
}

template <typename T, typename... Ts>
T construct_from_tuple(const std::tuple<Ts...>& tuple) {
    return detail::construct_from_tuple<T>(tuple, std::make_index_sequence<sizeof...(Ts)>());
}

} // namespace sl::meta
