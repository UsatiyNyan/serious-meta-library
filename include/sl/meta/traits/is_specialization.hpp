//
// Created by usatiynyan.
//

#pragma once

#include <type_traits>

namespace sl::meta {

template <typename T, template <typename...> class Template>
struct is_specialization : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {};

template <typename T, template <typename...> class Template>
constexpr bool is_specialization_v = is_specialization<T, Template>::value;

template <typename T, template <typename...> typename Template>
concept IsSpecialization = is_specialization_v<T, Template>;

} // namespace sl::meta
