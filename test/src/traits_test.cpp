//
// Created by usatiynyan on 8/1/23.
//

#include "sl/meta/traits/traits.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

using some_tuple_t = std::tuple<std::optional<std::string>, std::optional<int>, std::optional<std::vector<std::string>>>;

TEST(traits, isNargsConstructible) {
    static_assert(is_nargs_constructible_v<some_tuple_t, std::nullopt_t, 3>);
    static_assert(!is_nargs_constructible_v<some_tuple_t, std::nullopt_t, 2>);
    static_assert(!is_nargs_constructible_v<some_tuple_t, std::string, 3>);
}

TEST(traits, isAnyNargsConstructible) {
    static_assert(is_any_nargs_constructible_v<some_tuple_t, 2>);
    static_assert(is_any_nargs_constructible_v<some_tuple_t, 3>);
    static_assert(!is_any_nargs_constructible_v<some_tuple_t, 4>);
}

} // namespace sl::meta
