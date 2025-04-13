//
// Created by usatiynyan.
//

#pragma once


#include <string_view>
#include <type_traits>


namespace sl::meta {
namespace detail {

template <typename Enum>
concept EnumConvertibleToString = //
    std::is_enum_v<Enum> //
    && requires(Enum e) {
           { enum_to_str(e) } -> std::convertible_to<std::string_view>;
       };

} // namespace detail

template <typename EnumT, typename UnderlyingT = std::underlying_type_t<EnumT>>
constexpr EnumT next(EnumT e, UnderlyingT diff = 1) {
    return static_cast<EnumT>(static_cast<UnderlyingT>(e) + diff);
};

} // namespace sl::meta
