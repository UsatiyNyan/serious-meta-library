//
// Created by usatiynyan.
//

#include "sl/meta/match.hpp"

#include "fixture/lifecycle.hpp"

#include <gtest/gtest.h>
#include <tl/expected.hpp>
#include <tl/optional.hpp>

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

TEST(match, pmatchVariant) {
    {
        const std::variant<int, std::string, double> variant = 42;
        const auto result = variant
                            | pmatch{
                                  [](int) { return 0; },
                                  [](std::string) { return 1; },
                                  [](double) { return 2; },
                              };
        EXPECT_EQ(result, 0);
    }
    {
        const std::variant<int, std::string, double> variant = "42";
        const auto result = variant
                            | pmatch{
                                  [](int) { return 0; },
                                  [](std::string) { return 1; },
                                  [](double) { return 2; },
                              };
        EXPECT_EQ(result, 1);
    }
    {
        const std::variant<int, std::string, double> variant = 42.0;
        const auto result = variant
                            | pmatch{
                                  [](int) { return 0; },
                                  [](std::string) { return 1; },
                                  [](double) { return 2; },
                              };
        EXPECT_EQ(result, 2);
    }
}

TEST(match, pmatchVariantLifecycle) {
    const auto guard = fixture::lifecycle::make_states_guard();

    {
        std::variant<int, fixture::lifecycle> variant = 42;
        const auto result_int = variant
                                | pmatch{
                                      [](int) { return 0; },
                                      [](fixture::lifecycle&) { return 1; },
                                  };
        EXPECT_EQ(result_int, 0);
        EXPECT_EQ(fixture::lifecycle::states["variant"].size(), 0);

        variant.emplace<fixture::lifecycle>("variant");

        const auto result_lvalue = variant
                                   | pmatch{
                                         [](int) { return 0; },
                                         [](fixture::lifecycle&) { return 1; },
                                     };
        EXPECT_EQ(result_lvalue, 1);
        const std::vector expected_lvalue_states{ fixture::lifecycle::state::constructed };
        EXPECT_EQ(fixture::lifecycle::states["variant"], expected_lvalue_states);

        const auto result_copy = variant
                                 | pmatch{
                                       [](int) { return 0; },
                                       [](fixture::lifecycle) { return 1; },
                                   };
        EXPECT_EQ(result_copy, 1);
        const std::vector expected_copy_states{ fixture::lifecycle::state::constructed,
                                                fixture::lifecycle::state::copy_constructed_from };
        EXPECT_EQ(fixture::lifecycle::states["variant"], expected_copy_states);
        const std::vector expected_tmp_copy_states{ fixture::lifecycle::state::copy_constructed,
                                                    fixture::lifecycle::state::destructed };
        EXPECT_EQ(fixture::lifecycle::states[fixture::lifecycle::copied_id("variant")], expected_tmp_copy_states);

        const auto result_move = std::move(variant)
                                 | pmatch{
                                       [](int) { return 0; },
                                       [](fixture::lifecycle) { return 1; },
                                   };
        EXPECT_EQ(result_move, 1);
        const std::vector expected_move_states{ fixture::lifecycle::state::constructed,
                                                fixture::lifecycle::state::copy_constructed_from,
                                                fixture::lifecycle::state::move_constructed_from };
        EXPECT_EQ(fixture::lifecycle::states["variant"], expected_move_states);
        const std::vector expected_tmp_move_states{ fixture::lifecycle::state::move_constructed,
                                                    fixture::lifecycle::state::destructed };
        EXPECT_EQ(fixture::lifecycle::states[fixture::lifecycle::moved_id("variant")], expected_tmp_move_states);
    }

    const std::vector expected_states{ fixture::lifecycle::state::constructed,
                                       fixture::lifecycle::state::copy_constructed_from,
                                       fixture::lifecycle::state::move_constructed_from,
                                       fixture::lifecycle::state::destructed };
    EXPECT_EQ(fixture::lifecycle::states["variant"], expected_states);
}

TEST(match, pmatchExpected) {
    {
        const tl::expected<int, std::string> expected = 42;
        const auto result = expected
                            | pmatch{
                                  [](int) { return 0; },
                                  [](const std::string&) { return 1; },
                              };
        EXPECT_EQ(result, 0);
    }
    {
        const tl::expected<int, std::string> expected = tl::unexpected("42");
        const auto result = expected
                            | pmatch{
                                  [](int) { return 0; },
                                  [](const std::string&) { return 1; },
                              };
        EXPECT_EQ(result, 1);
    }
}

TEST(match, pmatchOptional) {
    {
        const tl::optional<int> optional = 42;
        const auto result = optional
                            | pmatch{
                                  [](int) { return 0; },
                                  []() { return 1; },
                              };
        EXPECT_EQ(result, 0);
    }
    {
        const tl::optional<int> optional = tl::nullopt;
        const auto result = optional
                            | pmatch{
                                  [](int) { return 0; },
                                  []() { return 1; },
                              };
        EXPECT_EQ(result, 1);
    }
}

TEST(match, pmatchPtr) {
    {
        const int* ptr = new int(42);
        defer ptr_delete{ [ptr] { delete ptr; } };

        const auto result = ptr
                            | pmatch{
                                  [](int) { return 0; },
                                  []() { return 1; },
                              };
        EXPECT_EQ(result, 0);
    }
    {
        const int* ptr = nullptr;
        const auto result = ptr
                            | pmatch{
                                  [](int) { return 0; },
                                  []() { return 1; },
                              };
        EXPECT_EQ(result, 1);
    }
}

TEST(match, pmatchPtrShared) {
    {
        const std::shared_ptr<int> ptr = std::make_shared<int>(42);
        const auto result = ptr
                            | pmatch{
                                  [](int) { return 0; },
                                  []() { return 1; },
                              };
        EXPECT_EQ(result, 0);
    }
    {
        const std::shared_ptr<int> ptr;
        const auto result = ptr
                            | pmatch{
                                  [](int) { return 0; },
                                  []() { return 1; },
                              };
        EXPECT_EQ(result, 1);
    }
}

TEST(match, pmatchPtrUnique) {
    {
        const std::unique_ptr<int> ptr = std::make_unique<int>(42);
        const auto result = ptr
                            | pmatch{
                                  [](int) { return 0; },
                                  []() { return 1; },
                              };
        EXPECT_EQ(result, 0);
    }
    {
        const std::unique_ptr<int> ptr;
        const auto result = ptr
                            | pmatch{
                                  [](int) { return 0; },
                                  []() { return 1; },
                              };
        EXPECT_EQ(result, 1);
    }
}

TEST(match, pmatchPtrLifecycle) {
    const auto guard = fixture::lifecycle::make_states_guard();
    {
        auto ptr = std::make_unique<fixture::lifecycle>("ptr");
        const auto result_lvalue = ptr
                                   | pmatch{
                                         [](fixture::lifecycle&) { return 0; },
                                         []() { return 1; },
                                     };
        EXPECT_EQ(result_lvalue, 0);
        const std::vector expected_lvalue_states{ fixture::lifecycle::state::constructed };
        EXPECT_EQ(fixture::lifecycle::states["ptr"], expected_lvalue_states);

        const auto result_copy = ptr
                                 | pmatch{
                                       [](fixture::lifecycle) { return 0; },
                                       []() { return 1; },
                                   };
        EXPECT_EQ(result_copy, 0);
        const std::vector expected_copy_states{ fixture::lifecycle::state::constructed,
                                                fixture::lifecycle::state::copy_constructed_from };
        EXPECT_EQ(fixture::lifecycle::states["ptr"], expected_copy_states);
        const std::vector expected_tmp_copy_states{ fixture::lifecycle::state::copy_constructed,
                                                    fixture::lifecycle::state::destructed };
        EXPECT_EQ(fixture::lifecycle::states[fixture::lifecycle::copied_id("ptr")], expected_tmp_copy_states);

        const auto result_move = std::move(ptr)
                                 | pmatch{
                                       [](fixture::lifecycle) { return 0; },
                                       []() { return 1; },
                                   };
        EXPECT_EQ(result_move, 0);
        const std::vector expected_move_states{ fixture::lifecycle::state::constructed,
                                                fixture::lifecycle::state::copy_constructed_from,
                                                fixture::lifecycle::state::move_constructed_from };
        EXPECT_EQ(fixture::lifecycle::states["ptr"], expected_move_states);
        const std::vector expected_tmp_move_states{ fixture::lifecycle::state::move_constructed,
                                                    fixture::lifecycle::state::destructed };
        EXPECT_EQ(fixture::lifecycle::states[fixture::lifecycle::moved_id("ptr")], expected_tmp_move_states);
    }
    const std::vector expected_states{ fixture::lifecycle::state::constructed,
                                       fixture::lifecycle::state::copy_constructed_from,
                                       fixture::lifecycle::state::move_constructed_from,
                                       fixture::lifecycle::state::destructed };
    EXPECT_EQ(fixture::lifecycle::states["ptr"], expected_states);
}

} // namespace sl::meta
