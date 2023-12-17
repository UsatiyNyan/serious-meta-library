//
// Created by usatiynyan on 12/17/23.
//

#pragma once

#include <type_traits>

namespace sl::meta {
namespace detail {

template <template <typename...> typename TupleT, typename... Ts>
constexpr auto decay_tuple(TupleT<Ts...>) -> TupleT<std::decay_t<Ts>...>;

} // namespace detail

template <typename T>
using decay_tuple_t = decltype(detail::decay_tuple(std::declval<T>()));

} // namespace sl::meta
