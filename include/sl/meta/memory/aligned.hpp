//
// Created by usatiynyan.
//

#pragma once

#include <cstddef>

namespace sl::meta {

template <typename T, typename AlignAsT>
struct aligned {
    static_assert(alignof(AlignAsT) >= sizeof(T));
    alignas(AlignAsT) T value;
};

template <typename T, std::size_t N, typename AlignAsT>
struct aligned<T[N], AlignAsT> {
    static_assert(alignof(AlignAsT) >= sizeof(T));
    alignas(AlignAsT) T value[N];
};

} // namespace sl::meta
