//
// Created by usatiynyan.
//

#pragma once

namespace sl::meta {

struct immovable {
    immovable() = default;

    immovable(const immovable&) = delete;
    immovable& operator=(const immovable&) = delete;

    immovable(immovable&&) = delete;
    immovable& operator=(immovable&&) = delete;
};

} // namespace sl::meta
