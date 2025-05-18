//
// Created by usatiynyan.
//

#pragma once

#include <concepts>
#include <type_traits>

namespace sl::meta {

struct unique {
    constexpr unique() = default;

    unique(const unique&) = delete;
    unique& operator=(const unique&) = delete;

    unique(unique&&) noexcept = default;
    unique& operator=(unique&&) noexcept = default;
};

struct immovable {
    immovable() = default;

    immovable(const immovable&) = delete;
    immovable& operator=(const immovable&) = delete;

    immovable(immovable&&) = delete;
    immovable& operator=(immovable&&) = delete;
};

template <typename T>
concept Unique = !std::copy_constructible<T> && !std::is_copy_assignable_v<T>;

template <typename T>
concept Immovable = Unique<T> && //
                    !std::move_constructible<T> && !std::is_move_assignable_v<T>;

} // namespace sl::meta
