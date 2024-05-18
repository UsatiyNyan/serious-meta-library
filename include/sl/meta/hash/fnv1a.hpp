//
// Created by usatiynyan on 8/6/23.
//

#pragma once

#include <cstdint>
#include <string_view>

namespace sl::meta {

namespace detail {

template <typename T>
struct fnv1a;

template <>
struct fnv1a<std::uint32_t> {
    static constexpr std::uint32_t offset = 2166136261u;
    static constexpr std::uint32_t prime = 16777619u;
};

template <>
struct fnv1a<std::uint64_t> {
    static constexpr std::uint64_t offset = 14695981039346656037ul;
    static constexpr std::uint64_t prime = 1099511628211ul;
};

} // namespace detail


/*
 * hash = offset_basis
 * for each octet_of_data to be hashed
 *     hash = hash xor octet_of_data
 *     hash = hash * FNV_prime
 * return hash
 * */
template <typename T>
constexpr T fnv1a(std::string_view string_view) {
    constexpr T offset = detail::fnv1a<T>::offset;
    constexpr T prime = detail::fnv1a<T>::prime;
    T hash = offset;
    for (auto c : string_view) {
        hash ^= static_cast<T>(c);
        hash *= prime;
    }
    return hash;
}

} // namespace sl::meta
