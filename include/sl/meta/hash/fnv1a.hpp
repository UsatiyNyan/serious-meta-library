//
// Created by usatiynyan on 8/6/23.
//

#pragma once

#include <cstdint>
#include <string_view>
#include <utility>

namespace sl::meta {

namespace detail {

/*
 * hash = offset_basis
 * for each octet_of_data to be hashed
 *     hash = hash xor octet_of_data
 *     hash = hash * FNV_prime
 * return hash
 * */

template <typename T, T offset, T prime, std::size_t... idxs>
constexpr T fnv1a(const char* s, std::index_sequence<idxs...>) {
    T hash = offset;
    ((hash = (hash ^ static_cast<T>(s[idxs])) * prime), ...);
    return hash;
}

template <typename T, T offset, T prime, std::size_t count>
constexpr T fnv1a(const char* s) {
    return fnv1a<T, offset, prime>(s, std::make_index_sequence<count - 1>());
}

template <std::size_t count>
constexpr std::uint32_t fnv1a_32(const char* s) {
    constexpr std::uint32_t FNV1A_32_OFFSET = 2166136261u;
    constexpr std::uint32_t FNV1A_32_PRIME = 16777619u;

    return detail::fnv1a<std::uint32_t, FNV1A_32_OFFSET, FNV1A_32_PRIME, count>(s);
}

template <std::size_t count>
constexpr std::uint64_t fnv1a_64(const char* s) {
    constexpr std::uint64_t FNV1A_64_OFFSET = 14695981039346656037ul;
    constexpr std::uint64_t FNV1A_64_PRIME = 1099511628211ul;

    return detail::fnv1a<std::uint64_t, FNV1A_64_OFFSET, FNV1A_64_PRIME, count>(s);
}

} // namespace detail

template <std::size_t count>
constexpr std::uint32_t fnv1a_32(const char (&s)[count]) {
    return detail::fnv1a_32<count>(s);
}

template <std::size_t count>
constexpr std::uint32_t fnv1a_32(const char s[count]) {
    return detail::fnv1a_32<count>(s);
}

template <std::size_t count>
constexpr std::uint64_t fnv1a_64(const char (&s)[count]) {
    return detail::fnv1a_64<count>(s);
}

template <std::size_t count>
constexpr std::uint64_t fnv1a_64(const char s[count]) {
    return detail::fnv1a_64<count>(s);
}

} // namespace sl::meta
