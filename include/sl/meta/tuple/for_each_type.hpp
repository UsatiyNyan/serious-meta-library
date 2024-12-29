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
    template <template <typename> typename F, typename... Args>
    constexpr static auto apply(Args&&... args) {
        if constexpr ((std::is_void_v<decltype(F<Ts>{}(std::forward<Args>(args)...))> || ...)) {
            (F<Ts>{}(std::forward<Args>(args)...), ...);
        } else {
            return std::make_tuple(F<Ts>{}(std::forward<Args>(args)...)...);
        }
    }
};

} // namespace detail

template <template <typename> typename F, typename TupleT, typename... Args>
constexpr auto for_each_type(Args&&... args) {
    return detail::for_each_type<TupleT>::template apply<F>(std::forward<Args>(args)...);
}

} // namespace sl::meta
