//
// Created by usatiynyan.
//

#pragma once


#include <type_traits>
#include <variant>

namespace sl::meta {

template <typename T, typename OriginalT = std::decay_t<T>>
concept treat_as_optional = //
    requires(T v) {
        { v.has_value() } -> std::same_as<bool>;
        { v.value() } -> std::convertible_to<typename OriginalT::value_type>;
    };

template <typename T, typename OriginalT = std::decay_t<T>>
concept treat_as_expected = //
    treat_as_optional<T> //
    && requires(T v) {
           { v.error() } -> std::convertible_to<typename OriginalT::error_type>;
       };

template <typename T>
concept treat_as_variant = //
    requires(T v) {
        {
            std::visit([](auto&&) {}, std::forward<T>(v))
        };
    };

template <typename T, typename OriginalT = std::decay_t<T>>
concept treat_as_smart_ptr = //
    requires(T v) {
        { v == nullptr } -> std::same_as<bool>;
        { *v } -> std::convertible_to<typename OriginalT::element_type>;
    };

template <typename T, typename OriginalT = std::decay_t<T>>
concept treat_as_ptr = std::is_pointer_v<OriginalT> || treat_as_smart_ptr<T>;

} // namespace sl::meta
