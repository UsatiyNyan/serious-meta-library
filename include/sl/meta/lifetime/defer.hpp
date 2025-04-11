//
// Created by usatiynyan on 1/2/24.
//

#pragma once

#include "sl/meta/lifetime/unique.hpp"

#include "sl/meta/func/function.hpp"

namespace sl::meta {
template <typename Capacity = fu2::capacity_default>
struct defer : unique {
    using impl_type = unique_function<void(), Capacity>;

public:
    defer(defer&& other) noexcept : f_{ std::exchange(other.f_, nullptr) } {}
    defer& operator=(defer&& other) noexcept {
        if (this != &other) {
            call();
            f_ = std::exchange(other.f_, nullptr);
        }
        return *this;
    }

    explicit defer(impl_type f) : f_(std::move(f)) {}
    ~defer() noexcept { call(); }

private:
    void call() noexcept {
        if (auto f = std::exchange(f_, nullptr)) {
            f();
        }
    }

private:
    impl_type f_{};
};

template <typename F>
defer(F) -> defer<fu2::capacity_can_hold<F>>;
} // namespace sl::meta
