//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/hash/fnv1a.hpp"

#include <string_view>

namespace sl::meta {

template <typename C>
class basic_hash_string_view {
public:
    explicit constexpr basic_hash_string_view(std::basic_string_view<C> sv)
        : sv_{ sv }, hash_{ fnv1a<std::uint64_t>(sv) } {}

    constexpr bool operator==(const basic_hash_string_view& other) const {
        return hash_ == other.hash_ && sv_ == other.sv_;
    }

    constexpr auto hash() const { return hash_; }
    constexpr auto string_view() const { return sv_; }

private:
    std::basic_string_view<C> sv_;
    std::uint64_t hash_;
};

using hash_string_view = basic_hash_string_view<char>;

constexpr hash_string_view operator""_hsv(const char* str, std::size_t len) {
    return hash_string_view{ std::string_view{ str, len } };
}

} // namespace sl::meta

template <typename C>
struct std::hash<sl::meta::basic_hash_string_view<C>> {
    constexpr auto operator()(const sl::meta::basic_hash_string_view<C>& value) const { return value.hash(); }
};
