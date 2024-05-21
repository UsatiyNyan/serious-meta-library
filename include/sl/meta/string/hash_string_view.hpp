//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/hash/fnv1a.hpp"

#include <string_view>

namespace sl::meta {

class hash_string_view {
public:
    explicit constexpr hash_string_view(std::string_view sv) : hash_{ fnv1a<std::uint64_t>(sv) }, sv_{ sv } {}

    constexpr bool operator==(const hash_string_view& other) const { return hash_ == other.hash_ && sv_ == other.sv_; }

    constexpr auto hash() const { return hash_; }
    constexpr auto string_view() const { return sv_; }

private:
    std::uint64_t hash_;
    std::string_view sv_;
};

constexpr hash_string_view operator""_hsv(const char* str, std::size_t len) {
    return hash_string_view{ std::string_view{ str, len } };
}

} // namespace sl::meta

template <>
struct std::hash<sl::meta::hash_string_view> {
    constexpr auto operator()(const sl::meta::hash_string_view& value) const { return value.hash(); }
};
