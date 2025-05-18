//
// Created by usatiynyan.
//

#pragma once

#include <type_traits>

namespace sl::meta::type {

template <typename T, bool cond>
using add_const_if = std::conditional_t<cond, std::add_const<T>, std::type_identity<T>>;

template <typename T, bool cond>
using add_const_if_t = typename add_const_if<T, cond>::type;

} // namespace sl::meta
