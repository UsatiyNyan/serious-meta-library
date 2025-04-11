//
// Created by usatiynyan.
//

#pragma once


#include "sl/meta/enum/to_string.hpp"
#include "sl/meta/match/match_map.hpp"
#include "to_string.hpp"

#include <string_view>
#include <type_traits>

namespace sl::meta {
namespace detail {

template <typename EnumT, typename = void>
struct has_enum_end : std::false_type {};

template <typename EnumT>
struct has_enum_end<EnumT, decltype(EnumT::ENUM_END)> : std::true_type {};

template <typename EnumT>
concept enum_convertible_from_string = //
    enum_convertible_to_string<EnumT> && //
    has_enum_end<EnumT>::value;

} // namespace detail

template <detail::enum_convertible_to_string EnumT>
constexpr auto make_enum_match_map() {
    return make_match_map<std::string_view, EnumT>([](auto match) {
        constexpr auto next = [](EnumT e) {
            using underlying_type = std::underlying_type_t<EnumT>;
            const auto u = static_cast<underlying_type>(e);
            return static_cast<EnumT>(u + 1);
        };

        for (EnumT e{}; e != EnumT::ENUM_END; e = next(e)) {
            std::string_view e_str = enum_to_str(e);
            if (e_str.empty()) {
                continue;
            }
            match = match.case_(e_str, e);
        }
        return match;
    });
}

template <detail::enum_convertible_to_string EnumT>
constexpr EnumT enum_from_str(std::string_view str) {
    constexpr auto m = make_enum_match_map<EnumT>();
    return m[str].value_or(EnumT::ENUM_END);
}

} // namespace sl::meta
