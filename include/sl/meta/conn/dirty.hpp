//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/monad/maybe.hpp"
#include "sl/meta/traits/unique.hpp"

#include <libassert/assert.hpp>

namespace sl::meta {

template <typename T>
class dirty : public unique {
public:
    constexpr explicit dirty() = default;
    constexpr explicit dirty(T value) : value_{ std::move(value) }, is_dirty_{ true } {}

    [[nodiscard]] constexpr maybe<const T&> get() const {
        if (!value_.has_value()) {
            return null;
        }
        return value_.value();
    }
    // would give value if it was changed, and mark it as unchanged for next "relase", until next "set"
    [[nodiscard]] constexpr maybe<const T&> release() {
        if (!std::exchange(is_dirty_, false)) {
            return null;
        }
        DEBUG_ASSERT(value_.has_value(), "invariant");
        return value_.value();
    }

    constexpr void set(T value) {
        value_.emplace(std::move(value));
        is_dirty_ = true;
    }

    constexpr void set_if_ne(T value) {
        if (value_ != value) {
            set(std::move(value));
        }
    }

    constexpr void set_dirty(bool is_dirty) { is_dirty_ = is_dirty; }

private:
    maybe<T> value_{};
    bool is_dirty_ = false;
};

} // namespace sl::meta
