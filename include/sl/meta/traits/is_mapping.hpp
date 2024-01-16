//
// Created by usatiynyan on 16.01.24.
//

#pragma once

#include "is_range.hpp"

#include <concepts>
#include <type_traits>

namespace sl::meta {

template <typename T>
struct is_mapping : std::false_type {};

template <typename T>
    requires is_range<T> && //
             requires(T v) {
                 typename T::key_type;
                 typename T::mapped_type;
                 typename T::value_type;
                 { v[std::declval<typename T::key_type>()] } -> std::same_as<typename T::mapped_type&>;
             }
struct is_mapping<T> : std::true_type {};

template <typename T>
constexpr bool is_mapping_v = is_mapping<T>::value;

} // namespace sl::meta
