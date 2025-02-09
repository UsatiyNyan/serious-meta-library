//
// Created by usatiynyan.
//

#pragma once

#include <tuple>
#include <type_traits>

namespace sl::meta::type {

template <typename T, typename...>
struct head {
    using type = T;
};

template <typename... Ts>
using head_t = typename head<Ts...>::type;

template <std::size_t Idx, typename... Ts>
using at_t = std::tuple_element_t<Idx, std::tuple<Ts...>>;

template <typename T, std::size_t N>
struct repeat {
    template <typename U, std::size_t>
    using impl_type = U;

    template <typename U, std::size_t... Idxs>
    static auto impl(std::index_sequence<Idxs...>) -> std::tuple<impl_type<U, Idxs>...>;

    using type = decltype(impl<T>(std::make_index_sequence<N>()));
};

template <typename T, std::size_t N>
using repeat_t = typename repeat<T, N>::type;

template <typename T, typename... Ts>
constexpr bool are_same_v = (std::is_same_v<T, Ts> && ... && true);

} // namespace sl::meta::type
