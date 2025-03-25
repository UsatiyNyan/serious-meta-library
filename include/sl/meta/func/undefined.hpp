//
// Created by usatiynyan.
//

#pragma once

#include <type_traits>

namespace sl::meta {

struct undefined {
    template <typename T>
    constexpr operator T() const {
        static_assert(!std::is_same_v<T, T>, "trying to evaluate undefined");
        return {};
    }
};

} // namespace sl::meta
