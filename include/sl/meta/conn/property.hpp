//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/conn/signal.hpp"
#include "sl/meta/traits/unique.hpp"

#include <concepts>
#include <utility>

namespace sl::meta {

template <typename T>
    requires std::equality_comparable<T> && std::movable<T>
class property : public unique {
public:
    property() = default;
    explicit property(T value) : value_{ std::move(value) } {}

    [[nodiscard]] const T& get() const { return value_; }
    void set(T value) {
        if (value_ != value) {
            value_ = std::move(value);
            changed_(value_);
        }
    }

    signal<const T&>& changed() { return changed_; }

private:
    T value_;
    signal<const T&> changed_;
};

} // namespace sl::meta
