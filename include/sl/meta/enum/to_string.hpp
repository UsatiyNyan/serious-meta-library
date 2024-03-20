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
    std::is_enum_v<Enum> && //
    requires(Enum e) {
        { enum_to_str(e) } -> std::convertible_to<std::string_view>;
    };

} // namespace detail
} // namespace sl::meta
