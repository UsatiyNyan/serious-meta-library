//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/traits/is_specialization.hpp"

#include <concepts>
#include <ranges>
#include <variant>

namespace sl::meta {

template <typename T>
concept SequenceContainerLike = std::ranges::range<T>&&
    requires(T)
{
    typename T::value_type;
};

template <typename T>
concept AssociativeContainerLike = SequenceContainerLike<T>&&
    requires(T)
{
    typename T::key_type;
    typename T::mapped_type;
}
&&requires(T m, T::key_type k) {
    { m[k] } -> std::same_as<typename T::mapped_type&>;
};

template <typename T>
concept OptionalLike = requires(T v) {
    typename T::value_type;

    { v.has_value() } -> std::same_as<bool>;
    { v.value() } -> std::convertible_to<typename T::value_type>;
};

template <typename T>
concept ExpectedLike = OptionalLike<T> && requires(T v) {
    typename T::error_type;

    { v.error() } -> std::convertible_to<typename T::error_type>;
};

template <typename T>
concept Variant = IsSpecialization<T, std::variant>;

template <typename T>
concept SmartPtrLike = requires(T v) {
    typename T::element_type;

    { v == nullptr } -> std::same_as<bool>;
    { *v } -> std::convertible_to<typename T::element_type>;
    { v.get() } -> std::same_as<typename T::element_type*>;
};

template <typename T>
concept PtrLike = std::is_pointer_v<T> || SmartPtrLike<T>;

} // namespace sl::meta
