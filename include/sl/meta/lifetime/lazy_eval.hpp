//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/lifetime/unique.hpp"

#include <type_traits>
#include <utility>

namespace sl::meta {

template <typename F, typename T = std::invoke_result_t<F>>
class lazy_eval : public unique {
public:
    constexpr explicit lazy_eval(F f) : f_{ std::move(f) } {}

    [[nodiscard]] constexpr operator T() && { return std::move(f_)(); }
    [[nodiscard]] constexpr T operator()() && { return std::move(f_)(); }

private:
    F f_;
};

template <typename T>
class identity_eval : public unique {
public:
    constexpr explicit identity_eval(T value) : value_{ std::move(value) } {}

    [[nodiscard]] constexpr operator T() && { return std::move(value_); }
    [[nodiscard]] constexpr T operator()() && { return std::move(value_); }

private:
    T value_;
};

} // namespace sl::meta
