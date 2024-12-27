//
// Created by usatiynyan.
//

#pragma once

#include <tuple>
#include <type_traits>

namespace sl::meta::type {

template <typename T, typename...>
using head_t = T;

template <std::size_t Idx, typename... Ts>
using at_t = std::tuple_element_t<Idx, std::tuple<Ts...>>;

template <typename T, typename... Ts>
constexpr bool are_same_v = (std::is_same_v<T, Ts> && ... && true);

} // namespace sl::meta::type
