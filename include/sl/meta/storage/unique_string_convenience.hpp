//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/storage/unique_string.hpp"

#include <fmt/format.h>

namespace sl::meta {

using unique_string = basic_unique_string<char>;
using unique_string_storage = basic_unique_string_storage<char>;

constexpr auto operator""_us(const char* str, std::size_t len) {
    return [hsv = hash_string_view{ std::string_view{ str, len } }](unique_string_storage& storage) {
        return storage.insert(hsv);
    };
}

constexpr auto operator""_ufs(const char* str, std::size_t len) {
    return [sv = std::string_view{ str, len }](auto&&... args) {
        return [fs = fmt::format(sv, std::forward<decltype(args)>(args)...)](unique_string_storage& storage) {
            return storage.insert(hash_string_view{ std::string_view{ fs } });
        };
    };
}

} // namespace sl::meta
