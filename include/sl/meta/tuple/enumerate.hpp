//
// Created by usatiynyan on 12/10/23.
//

#pragma once

#include <tuple>

namespace sl::meta {
namespace detail {

template <typename TupleTV, std::size_t... idxs>
auto enumerate(TupleTV&& tuple, std::index_sequence<idxs...>) {
    return std::tuple{
        std::make_tuple(
            /* .0 = */ idxs,
            /* .1 = */ std::get<idxs>(std::forward<TupleTV>(tuple))
        )... //
    };
}

} // namespace detail

template <typename TupleTV>
auto enumerate(TupleTV&& tuple) {
    using TupleT = std::decay_t<TupleTV>;
    constexpr std::size_t tuple_size = std::tuple_size_v<TupleT>;
    return detail::enumerate(std::forward<TupleTV>(tuple), std::make_index_sequence<tuple_size>());
}

} // namespace sl::meta
