//
// Created by usatiynyan on 14.07.23.
//

#pragma once

#include <algorithm>
#include <string_view>

namespace sl::meta {

template<std::size_t N>
struct string {
    constexpr string(const char (&literal_str)[N]) { // NOLINT(google-explicit-constructor)
        std::copy_n(literal_str, N, chars);
    }
    constexpr auto string_view() const { return std::string_view{ chars }; }

public:
    char chars[N] = {};
};

} // namespace sl::meta
