//
// Created by usatiynyan.
//

#pragma once

#include <type_traits>

namespace sl::meta {

template <typename F, typename R, typename... Args>
concept Invocable = std::is_invocable_r_v<R, F, Args...>;

template <typename F, typename R, typename... Args>
concept NothrowInvocable = std::is_nothrow_invocable_r_v<R, F, Args...>;

} // namespace sl::meta
