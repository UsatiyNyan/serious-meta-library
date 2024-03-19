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

TEST(match, bimatch) {
    constexpr auto use = [](bimatch<std::string_view, int> m) {
        return m //
            .case_("a", 1)
            .case_("b", 2)
            .case_("c", 3);
    };

    constexpr bimatch<std::string_view, int> key_match_a{ "a" };
    constexpr bimatch<std::string_view, int> key_match_b{ "b" };
    constexpr bimatch<std::string_view, int> key_match_c{ "c" };
    constexpr bimatch<std::string_view, int> key_match_d{ "" };

    static_assert(use(key_match_a).value() == 1);
    static_assert(use(key_match_b).value() == 2);
    static_assert(use(key_match_c).value() == 3);
    static_assert(use(key_match_d).value() == tl::nullopt);

    constexpr bimatch<std::string_view, int> value_match_1{ 1 };
    constexpr bimatch<std::string_view, int> value_match_2{ 2 };
    constexpr bimatch<std::string_view, int> value_match_3{ 3 };
    constexpr bimatch<std::string_view, int> value_match_4{ 4 };

    static_assert(use(value_match_1).key() == std::string_view{ "a" });
    static_assert(use(value_match_2).key() == std::string_view{ "b" });
    static_assert(use(value_match_3).key() == std::string_view{ "c" });
    static_assert(use(value_match_4).key() == tl::nullopt);
}

TEST(match, bimatchMap) {
    constexpr auto m = make_bimatch_map<std::string_view, int>([](auto match) {
        return match //
            .case_("a", 1)
            .case_("b", 2)
            .case_("c", 3);
    });

    static_assert(m.value("a") == 1);
    static_assert(m.value("b") == 2);
    static_assert(m.value("c") == 3);
    static_assert(m.value("d") == tl::nullopt);

    static_assert(m.key(1) == std::string_view{ "a" });
    static_assert(m.key(2) == std::string_view{ "b" });
    static_assert(m.key(3) == std::string_view{ "c" });
    static_assert(m.key(4) == tl::nullopt);
}

} // namespace sl::meta
