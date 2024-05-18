//
// Created by usatiynyan.
//

#pragma once

#include <algorithm>
#include <string_view>

namespace sl::meta {

template<std::size_t N>
struct static_string {
    constexpr static_string(const char (&literal_str)[N]) { // NOLINT(google-explicit-constructor)
        std::copy_n(literal_str, N, chars);
    }
    constexpr auto string_view() const { return std::string_view{ chars }; }

public:
    char chars[N] = {};
};

} // namespace sl::meta
