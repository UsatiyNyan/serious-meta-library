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
        return [fs = fmt::format(fmt::runtime(sv), std::forward<decltype(args)>(args)...)] //
            (unique_string_storage & storage) { return storage.insert(hash_string_view{ std::string_view{ fs } }); };
    };
}

} // namespace sl::meta

template <typename C, template <typename> typename Traits, template <typename> typename Alloc>
struct fmt::formatter<sl::meta::basic_unique_string<C, Traits, Alloc>>
    : fmt::formatter<std::basic_string_view<C, Traits<C>>> {
    template <typename Context>
    constexpr format_context::iterator format(const sl::meta::basic_unique_string<C, Traits, Alloc>& us, Context& ctx) const {
        return fmt::formatter<std::basic_string_view<C, Traits<C>>>::format(us.string_view(), ctx);
    }
};
