//
// Created by usatiynyan on 8/1/23.
//

#include "sl/meta/traits.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

struct TestStructOfOpts {
    std::optional<std::string> s;
    std::optional<int> i;
    std::optional<std::vector<std::string>> vs;
};

TEST(traits, isNargsConstructible) {
    static_assert(is_nargs_constructible_v<TestStructOfOpts, std::nullopt_t, 2>);
    static_assert(is_nargs_constructible_v<TestStructOfOpts, std::nullopt_t, 3>);
    static_assert(!is_nargs_constructible_v<TestStructOfOpts, std::string, 3>);
}

struct TestStruct {
    int i;
    std::string s;
    std::vector<int> vi;
};


TEST(traits, isAnyNargsConstructible) {
    static_assert(is_any_nargs_constructible_v<TestStruct, 1>);
    static_assert(is_any_nargs_constructible_v<TestStruct, 2>);
    static_assert(is_any_nargs_constructible_v<TestStruct, 3>);
    static_assert(!is_any_nargs_constructible_v<TestStruct, 4>);
}

TEST(traits, countFields) { static_assert(count_fields<TestStruct>() == 3); }

struct TestStructEmpty {};

TEST(traits, countFieldsEmpty) { static_assert(count_fields<TestStructEmpty>() == 0); }

} // namespace sl::meta
