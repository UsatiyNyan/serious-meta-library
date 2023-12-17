//
// Created by usatiynyan on 15.07.23.
//

#pragma once

#include <type_traits>
#include <utility>

namespace sl::meta {
namespace detail {

template <typename AggregateT, typename... TArgs>
    requires std::is_aggregate_v<AggregateT>
class is_constructible {
private:
    template <typename U, typename... UArgs>
    static auto test(int) -> decltype(U{ std::declval<UArgs>()... }, std::true_type{});

    template <typename, typename...>
    static auto test(...) -> std::false_type;

public:
    static constexpr bool value = decltype(test<AggregateT, TArgs...>(0))::value;
};

template <typename T, typename TArg, std::size_t... Idxs>
constexpr auto is_nargs_constructible(std::index_sequence<Idxs...>)
    -> is_constructible<T, decltype(Idxs, std::declval<TArg>())...>;

struct any_convertible {
    template <typename T>
    constexpr operator T() const noexcept;
};

} // namespace detail

template <typename T, typename TArg, std::size_t N>
using is_nargs_constructible = decltype(detail::is_nargs_constructible<T, TArg>(std::make_index_sequence<N>()));

template <typename T, typename TArg, std::size_t N>
constexpr bool is_nargs_constructible_v = is_nargs_constructible<T, TArg, N>::value;

template <typename T, std::size_t N>
using is_any_nargs_constructible = is_nargs_constructible<T, detail::any_convertible, N>;

template <typename T, std::size_t N>
constexpr bool is_any_nargs_constructible_v = is_any_nargs_constructible<T, N>::value;

} // namespace sl::meta
