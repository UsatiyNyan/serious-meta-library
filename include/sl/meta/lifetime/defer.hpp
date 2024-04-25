//
// Created by usatiynyan on 1/2/24.
//

#pragma once

#include "sl/meta/lifetime/unique.hpp"

#include <function2/function2.hpp>

namespace sl::meta {
class defer : public unique {
    using defer_function_t = fu2::function_base<
        /*IsOwning=*/true,
        /*IsCopyable=*/false,
        /*Capacity=*/fu2::capacity_default,
        /*IsThrowing=*/false,
        /*HasStrongExceptGuarantee=*/true,
        /*Signatures=*/void()>;

public:
    defer(defer&& other) noexcept : f_(std::move(other.f_)) { other.f_ = nullptr; }
    defer& operator=(defer&& other) noexcept {
        if (this != &other) {
            call();
            f_ = std::move(other.f_);
            other.f_ = nullptr;
        }
        return *this;
    }

    explicit defer(defer_function_t f) : f_(std::move(f)) {}
    ~defer() noexcept { call(); }

private:
    void call() noexcept {
        if (f_) {
            f_();
            f_ = nullptr;
        }
    }

private:
    defer_function_t f_{};
};
} // namespace sl::meta
