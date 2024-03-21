//
// Created by usatiynyan.
//

#pragma once


#include "to_string.hpp"

#include "sl/meta/match/match_map.hpp"

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
constexpr auto make_enum_match_map() {
    return make_match_map<std::string_view, Enum>([](auto match) {
        for (Enum e = Enum{}; e != Enum::ENUM_END;
             e = static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(e) + 1)) {
            constexpr std::string_view e_str = enum_to_str(e);
            if (e_str.empty()) {
                continue;
            }
            match = match.case_(e_str, e);
        }
        return match;
    });
}

template <detail::EnumConvertibleFromString Enum>
constexpr Enum enum_from_str(std::string_view str) {
    constexpr auto m = make_enum_match_map<Enum>();
    return m[str].value_or(Enum::ENUM_END);
}

} // namespace sl::meta
