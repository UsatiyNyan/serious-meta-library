//
// Created by usatiynyan on 7/30/23.
//

#pragma once

#include <tuple>

namespace sl::meta {
namespace detail {

template <typename F, typename TupleT, std::size_t... idxs>
auto for_each(F&& func, TupleT& tuple, std::index_sequence<idxs...>) {
    if constexpr ((std::is_void_v<decltype(func(std::get<idxs>(tuple)))> || ...)) {
        (std::forward<F>(func)(std::get<idxs>(tuple)), ...);
    } else {
        return std::tuple{ std::forward<F>(func)(std::get<idxs>(tuple))... };
    }
}

template <typename F, typename TupleT, std::size_t... idxs>
auto for_each(F&& func, const TupleT& tuple, std::index_sequence<idxs...>) {
    if constexpr ((std::is_void_v<decltype(func(std::get<idxs>(tuple)))> || ...)) {
        (std::forward<F>(func)(std::get<idxs>(tuple)), ...);
    } else {
        return std::tuple{ std::forward<F>(func)(std::get<idxs>(tuple))... };
    }
}

} // namespace detail

template <typename F, template <typename...> typename TupleT, typename... Ts>
auto for_each(F&& func, TupleT<Ts...>& tuple) {
    return detail::for_each(std::forward<F>(func), tuple, std::make_index_sequence<sizeof...(Ts)>());
}

template <typename F, template <typename...> typename TupleT, typename... Ts>
auto for_each(F&& func, const TupleT<Ts...>& tuple) {
    return detail::for_each(std::forward<F>(func), tuple, std::make_index_sequence<sizeof...(Ts)>());
}

} // namespace sl::meta
