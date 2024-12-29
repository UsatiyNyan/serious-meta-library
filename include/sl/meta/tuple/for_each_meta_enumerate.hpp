//
// Created by usatiynyan on 12/10/23.
//

#pragma once

#include <tuple>

namespace sl::meta {
namespace detail {

template <template <std::size_t> typename F, typename TupleTV, std::size_t... idxs>
auto for_each_meta_enumerate(TupleTV&& tuple, std::index_sequence<idxs...>) {
    if constexpr ((std::is_void_v<decltype(F<idxs>{}(std::get<idxs>(std::forward<TupleTV>(tuple))))> || ...)) {
        (F<idxs>{}(std::get<idxs>(std::forward<TupleTV>(tuple))), ...);
    } else {
        return std::make_tuple(F<idxs>{}(std::get<idxs>(std::forward<TupleTV>(tuple)))...);
    }
}

} // namespace detail

template <template <std::size_t> typename F, typename TupleTV>
auto for_each_meta_enumerate(TupleTV&& tuple) {
    using TupleT = std::decay_t<TupleTV>;
    constexpr std::size_t tuple_size = std::tuple_size_v<TupleT>;
    return detail::for_each_meta_enumerate<F>(std::forward<TupleTV>(tuple), std::make_index_sequence<tuple_size>());
}

} // namespace sl::meta
