//
// Created by usatiynyan.
//

#pragma once


#include "to_string.hpp"

#include <string_view>
#include <type_traits>


namespace sl::meta {
namespace detail {

template <typename Enum, typename = void>
struct has_enum_end : std::false_type {};

template <typename Enum>
struct has_enum_end<Enum, decltype(Enum::ENUM_END)> : std::true_type {};

template <typename Enum>
concept EnumConvertibleFromString = //
    EnumConvertibleToString<Enum> && //
    has_enum_end<Enum>::value;

} // namespace detail

template <detail::EnumConvertibleFromString Enum>
constexpr Enum enum_from_str(std::string_view str) {
    for (Enum e = Enum{}; e != Enum::ENUM_END;
         e = static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(e) + 1)) {
        if (enum_to_str(e) == str) {
            return e;
        }
    }
    return Enum::ENUM_END;
}

} // namespace sl::meta
