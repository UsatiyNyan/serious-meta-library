//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/hash/fnv1a.hpp"

#include <string_view>

namespace sl::meta {

struct hash_string_view {
    std::uint64_t hash;
    std::string_view string_view;
    constexpr bool operator==(const hash_string_view&) const = default;
};

constexpr hash_string_view operator""_hsv(const char* str, std::size_t len) {
    std::string_view string_view{ str, len };
    return hash_string_view{ .hash = fnv1a<uint64_t>(string_view), .string_view = string_view };
}

struct hash_string_view_op {
    constexpr auto operator()(const hash_string_view& value) const { return value.hash; }
};

} // namespace sl::meta

template <>
struct std::hash<sl::meta::hash_string_view> : public sl::meta::hash_string_view_op {
    using sl::meta::hash_string_view_op::operator();
};
