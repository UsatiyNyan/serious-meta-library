//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/lifetime/unique.hpp"

#include <tl/optional.hpp>

#include <functional>
#include <utility>

namespace sl::meta {

template <typename T>
class dirty : public unique {
public:
    constexpr dirty() = default;
    constexpr explicit dirty(T value, bool is_changed = true) : value_{ std::move(value) }, is_changed_{ is_changed } {}

    [[nodiscard]] constexpr const T& get() const { return value_; }
    constexpr void set(T value) {
        if (value_ != value) {
            value_ = std::move(value);
            is_changed_ = true;
        }
    }

    template <typename F, typename Ret = std::invoke_result_t<F, const T&>>
        requires(!std::is_void_v<Ret>)
    constexpr tl::optional<Ret> then(F&& f) {
        if (std::exchange(is_changed_, false)) {
            return std::forward<F>(f)(std::cref(value_));
        }
        return tl::nullopt;
    }

    template <typename F, typename Ret = std::invoke_result_t<F, const T&>>
        requires std::is_void_v<Ret>
    constexpr void then(F&& f) {
        if (std::exchange(is_changed_, false)) {
            std::forward<F>(f)(std::cref(value_));
        }
    }

    tl::optional<T> release() {
        if (is_changed_) {
            return std::move(value_);
        }
        return tl::nullopt;
    }

private:
    T value_{};
    bool is_changed_ = false;
};

} // namespace sl::meta
