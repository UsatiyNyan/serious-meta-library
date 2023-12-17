//
// Created by usatiynyan on 12/17/23.
//

#pragma once


#include <tuple>

namespace sl::meta {
namespace detail {

template <typename TupleT = void>
struct for_each_type;

template <template <typename...> typename TupleT, typename... Ts>
struct for_each_type<TupleT<Ts...>> {
    template <template <typename> typename F>
    constexpr static auto apply() {
        if constexpr ((std::is_void_v<decltype(F<Ts>{}())> || ...)) {
            (F<Ts>{}(), ...);
        } else {
            return std::tuple{ F<Ts>{}()... };
        }
    }
};

} // namespace detail

template <template <typename> typename F, typename TupleT>
constexpr auto for_each_type() {
    return detail::for_each_type<TupleT>::template apply<F>();
}

} // namespace sl::meta
