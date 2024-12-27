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

template <std::size_t, typename>
struct at;

template <typename T, typename... Ts>
struct at<0, list<T, Ts...>> {
    using type = T;
};

template <std::size_t Idx, typename T, typename... Ts>
struct at<Idx, list<T, Ts...>> {
    using type = typename at<Idx - 1, list<Ts...>>::type;
};

} // namespace detail

template <typename... Ts>
using head_t = typename detail::head<detail::list<Ts...>>::type;

template <std::size_t Idx, typename... Ts>
using at_t = typename detail::at<Idx, detail::list<Ts...>>::type;

template <typename T, typename... Ts>
constexpr bool are_same_v = (std::is_same_v<T, Ts> && ... && true);

} // namespace sl::meta::type
