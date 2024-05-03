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
    dirty() = default;
    explicit dirty(T value) : value_{ std::move(value) }, is_changed_{ true } {}

    [[nodiscard]] const T& get() const { return value_; }
    void set(T value) {
        if (value_ != value) {
            value_ = std::move(value);
            is_changed_ = true;
        }
    }

    template <typename F, typename Ret = std::invoke_result_t<F, const T&>>
        requires(!std::is_void_v<Ret>)
    tl::optional<Ret> then(F&& f) {
        if (std::exchange(is_changed_, false)) {
            return std::forward<F>(f)(std::cref(value_));
        }
        return tl::nullopt;
    }

private:
    T value_;
    bool is_changed_ = false;
};

} // namespace sl::meta
