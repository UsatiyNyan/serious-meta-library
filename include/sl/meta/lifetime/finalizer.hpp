//
// Created by usatiynyan on 11/18/23.
//

#pragma once

#include <function2/function2.hpp>

namespace sl {
// CRTP
template <typename Self>
class finalizer {
    using finalizer_function_t = fu2::function_base<
        /*IsOwning=*/true,
        /*IsCopyable=*/false,
        /*Capacity=*/fu2::capacity_default,
        /*IsThrowing=*/false,
        /*HasStrongExceptGuarantee=*/true,
        /*Signatures=*/void(Self&)>;

public:
    finalizer(const finalizer&) = delete;
    finalizer& operator=(const finalizer&) = delete;

    finalizer(finalizer&& other) noexcept : f_(std::move(other.f_)) { other.f_ = nullptr; }
    finalizer& operator=(finalizer&& other) noexcept {
        if (this != &other) {
            call();
            f_ = std::move(other.f_);
            other.f_ = nullptr;
        }
        return *this;
    }

    explicit finalizer(finalizer_function_t f) : f_(std::move(f)) {}
    ~finalizer() noexcept { call(); }

private:
    void call() noexcept {
        if (f_) {
            f_(*static_cast<Self*>(this));
            f_ = nullptr;
        }
    }

private:
    finalizer_function_t f_{};
};
} // namespace sl
