//
// Created by usatiynyan.
//

#include "sl/meta/match.hpp"

#include <gtest/gtest.h>


namespace sl::meta {

TEST(match, match) {
    constexpr auto use = [](match<std::string_view, int> m) {
        return m //
            .case_("a", 1)
            .case_("b", 2)
            .case_("c", 3);
    };

    constexpr match<std::string_view, int> match_a{ "a" };
    constexpr match<std::string_view, int> match_b{ "b" };
    constexpr match<std::string_view, int> match_c{ "c" };
    constexpr match<std::string_view, int> match_d{ {} };

    static_assert(use(match_a).result() == 1);
    static_assert(use(match_b).result() == 2);
    static_assert(use(match_c).result() == 3);
    static_assert(use(match_d).result() == tl::nullopt);
}

TEST(match, matchMap) {
    constexpr auto m = make_match_map<std::string_view, int>([](auto match) {
        return match //
            .case_("a", 1)
            .case_("b", 2)
            .case_("c", 3);
    });
    static_assert(m["a"] == 1);
    static_assert(m["b"] == 2);
    static_assert(m["c"] == 3);
    static_assert(m["d"] == tl::nullopt);
}

} // namespace sl::meta
