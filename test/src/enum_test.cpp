//
// Created by usatiynyan.
//

#include "sl/meta/enum.hpp"

#include <cstdint>
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

enum class TestFlagEnum : std::uint8_t {
    NONE = 0,
    FLAG0 = 1 << 0,
    FLAG1 = 1 << 1,
    FLAG2 = 1 << 2,
};

TEST(enum, flag) {
    constexpr enum_flag flag000{ TestFlagEnum::NONE };
    constexpr enum_flag flag001{ TestFlagEnum::FLAG0 };
    constexpr enum_flag flag010{ TestFlagEnum::FLAG1 };
    constexpr enum_flag flag100{ TestFlagEnum::FLAG2 };
    constexpr auto flag011 = flag001 | flag010;

    static_assert(!(flag011 & flag000));
    static_assert(flag011 & flag001);
    static_assert(flag011 & flag010);
    static_assert(!(flag011 & flag100));
}

} // namespace sl::meta
