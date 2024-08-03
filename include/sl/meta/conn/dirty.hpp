//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/lifetime/unique.hpp"

#include <libassert/assert.hpp>
#include <tl/optional.hpp>

namespace sl::meta {

template <typename T>
class dirty : public unique {
public:
    constexpr explicit dirty() = default;
    constexpr explicit dirty(T value) : value_{ std::move(value) }, is_changed_{ true } {}

    [[nodiscard]] constexpr tl::optional<const T&> get() const {
        if (!value_.has_value()) {
            return tl::nullopt;
        }
        return value_.value();
    }
    // would give value if it was changed, and mark it as unchanged for next "relase", until next "set"
    [[nodiscard]] constexpr tl::optional<const T&> release() {
        if (!std::exchange(is_changed_, false)) {
            return tl::nullopt;
        }
        DEBUG_ASSERT(value_.has_value(), "invariant");
        return value_.value();
    }

    constexpr void set(T value) {
        value_.emplace(std::move(value));
        is_changed_ = true;
    }

    constexpr void set_if_ne(T value) {
        if (value_ != value) {
            set(std::move(value));
        }
    }

private:
    tl::optional<T> value_{};
    bool is_changed_ = false;
};

} // namespace sl::meta
