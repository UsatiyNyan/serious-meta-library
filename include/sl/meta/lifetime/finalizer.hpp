//
// Created by usatiynyan on 11/18/23.
//

#pragma once

#include "sl/meta/lifetime/unique.hpp"

#include "sl/meta/func/function.hpp"

namespace sl::meta {
// CRTP
template <typename Self>
struct finalizer : unique {
    using impl_type = unique_function<void(Self&)>;

public:
    finalizer(finalizer&& other) noexcept : f_{ std::exchange(other.f_, nullptr) } {}
    finalizer& operator=(finalizer&& other) noexcept {
        if (this != &other) {
            call();
            f_ = std::exchange(other.f_, nullptr);
        }
        return *this;
    }

    explicit finalizer(impl_type f) : f_(std::move(f)) {}
    ~finalizer() noexcept { call(); }

private:
    void call() noexcept {
        if (auto f = std::exchange(f_, nullptr)) {
            f(*static_cast<Self*>(this));
        }
    }

private:
    impl_type f_{};
};
} // namespace sl::meta
