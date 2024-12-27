//
// Created by usatiynyan.
//

#pragma once

#include <type_traits>

namespace sl::meta::type {
namespace detail {

template <typename... Ts>
struct list {};

template <typename>
struct head;

template <typename T, typename... Ts>
struct head<list<T, Ts...>> {
    using type = T;
};

} // namespace detail

template <typename... Ts>
using head_t = typename detail::head<Ts...>::type;

template <typename T, typename... Ts>
constexpr bool are_same_v = (std::is_same_v<T, Ts> && ...);

} // namespace sl::meta::type
