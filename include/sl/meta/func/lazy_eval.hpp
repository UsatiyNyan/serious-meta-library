//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/traits/unique.hpp"

#include <type_traits>
#include <utility>

namespace sl::meta {

template <typename F>
struct lazy_eval : unique {
    using value_type = std::invoke_result_t<F>;

public:
    constexpr explicit lazy_eval(F f) : f_{ std::move(f) } {}

    [[nodiscard]] constexpr operator value_type() && { return std::move(f_)(); }
    [[nodiscard]] constexpr value_type operator()() && { return std::move(f_)(); }

private:
    F f_;
};

template <typename T>
struct identity_eval : unique {
    using value_type = T;

public:
    constexpr explicit identity_eval(T value) : value_{ std::move(value) } {}

    [[nodiscard]] constexpr operator T() && { return std::move(value_); }
    [[nodiscard]] constexpr T operator()() && { return std::move(value_); }

private:
    T value_;
};

} // namespace sl::meta
