//
// Created by usatiynyan.
//

#pragma once

namespace sl::meta {

template <typename>
concept False = false;

struct undefined {
    template <False T = void>
    constexpr undefined() {}
};

} // namespace sl::meta
