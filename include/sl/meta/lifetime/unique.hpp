//
// Created by usatiynyan.
//

#pragma once

namespace sl::meta {

struct unique {
    unique() = default;

    unique(const unique&) = delete;
    unique& operator=(const unique&) = delete;

    unique(unique&&) = default;
    unique& operator=(unique&&) = default;
};

} // namespace sl::meta
