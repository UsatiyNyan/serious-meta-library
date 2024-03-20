//
// Created by usatiynyan.
//

#include "sl/meta/enum.hpp"

#include <gtest/gtest.h>


namespace sl::meta {

enum class TestEnum { A, B, C, ENUM_END };

constexpr std::string_view enum_to_str(TestEnum e) {
    switch (e) {
    case TestEnum::A:
        return "A";
    case TestEnum::B:
        return "B";
    case TestEnum::C:
        return "C";
    default:
        return {};
    }
}

TEST(enum, fromString) {
    static_assert(enum_from_str<TestEnum>("A") == TestEnum::A);
    static_assert(enum_from_str<TestEnum>("B") == TestEnum::B);
    static_assert(enum_from_str<TestEnum>("C") == TestEnum::C);
    static_assert(enum_from_str<TestEnum>("D") == TestEnum::ENUM_END);
}

} // namespace sl::meta
