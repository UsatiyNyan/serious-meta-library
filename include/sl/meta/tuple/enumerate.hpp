//
// Created by usatiynyan on 12/10/23.
//

#pragma once

#pragma once

#include <tuple>

namespace sl::meta {
namespace detail {

template <typename TupleT, std::size_t... idxs>
auto enumerate(const TupleT& tuple, std::index_sequence<idxs...>)
    -> std::tuple<std::tuple<std::size_t, std::tuple_element_t<idxs, TupleT>>...> {
    return std::tuple{ std::tuple{ idxs, std::get<idxs>(tuple) }... };
}

} // namespace detail

template <template <typename...> typename TupleT, typename... Ts>
auto enumerate(const TupleT<Ts...>& tuple) {
    return detail::enumerate(tuple, std::make_index_sequence<sizeof...(Ts)>());
}

} // namespace sl::meta
