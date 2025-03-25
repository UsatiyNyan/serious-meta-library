//
// Created by usatiynyan.
//

#pragma once

namespace sl::meta {

struct unique {
    constexpr unique() = default;

    unique(const unique&) = delete;
    unique& operator=(const unique&) = delete;

    unique(unique&&) noexcept = default;
    unique& operator=(unique&&) noexcept = default;
};

} // namespace sl::meta
