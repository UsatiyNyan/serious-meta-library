//
// Created by usatiynyan on 12/10/23.
//

#pragma once

#include <tuple>

namespace sl::meta {
namespace detail {

template <template <std::size_t> typename F, typename TupleT, std::size_t... idxs>
auto for_each_meta_enumerate(TupleT& tuple, std::index_sequence<idxs...>) {
    if constexpr ((std::is_void_v<decltype(F<idxs>{}(std::get<idxs>(tuple)))> || ...)) {
        (F<idxs>{}(std::get<idxs>(tuple)), ...);
    } else {
        return std::tuple{ F<idxs>{}(std::get<idxs>(tuple))... };
    }
}

} // namespace detail

template <template <std::size_t> typename F, typename... Ts>
auto for_each_meta_enumerate(std::tuple<Ts...>& tuple) {
    return detail::for_each_meta_enumerate<F>(tuple, std::make_index_sequence<sizeof...(Ts)>());
}

template <template <std::size_t> typename F, typename... Ts>
auto for_each_meta_enumerate(const std::tuple<Ts...>& tuple) {
    return detail::for_each_meta_enumerate<F>(tuple, std::make_index_sequence<sizeof...(Ts)>());
}

} // namespace sl::meta
