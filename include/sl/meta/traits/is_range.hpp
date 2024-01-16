//
// Created by usatiynyan on 16.01.24.
//

#pragma once

#include <iterator>

namespace sl::meta {

template <typename T>
concept is_range = //
    requires(T v) {
        { begin(v) } -> std::input_or_output_iterator;
        { end(v) } -> std::sentinel_for<decltype(begin(v))>;
    };

template <typename T>
constexpr bool is_range_v = is_range<T>;

} // namespace sl::meta
