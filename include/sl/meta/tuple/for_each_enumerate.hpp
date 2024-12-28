//
// Created by usatiynyan on 12/10/23.
//

#pragma once

#include <tuple>

namespace sl::meta {
namespace detail {

template <typename FV, typename TupleTV, std::size_t... idxs>
auto for_each_enumerate(FV&& func, TupleTV& tuple, std::index_sequence<idxs...>) {
    if constexpr ((std::is_void_v<decltype(func(std::get<idxs>(std::forward<TupleTV>(tuple))))> || ...)) {
        (std::forward<FV>(func)(idxs, std::get<idxs>(std::forward<TupleTV>(tuple))), ...);
    } else {
        return std::tuple{ std::forward<FV>(func)(idxs, std::get<idxs>(std::forward<TupleTV>(tuple)))... };
    }
}

} // namespace detail

template <typename FV, typename TupleTV>
auto for_each_enumerate(FV&& func, TupleTV&& tuple) {
    using TupleT = std::decay_t<TupleTV>;
    constexpr std::size_t tuple_size = std::tuple_size_v<TupleT>;
    return detail::for_each_enumerate(
        std::forward<FV>(func), std::forward<TupleTV>(tuple), std::make_index_sequence<tuple_size>()
    );
}

} // namespace sl::meta
