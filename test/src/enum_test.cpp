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

enum class TestFlagEnum : std::uint32_t {
    NONE = 0,
    FLAG0 = 1u << 0,
    FLAG1 = 1u << 1,
    FLAG2 = 1u << 31,
};

TEST(enum, flag) {
    constexpr enum_flag flag000{ TestFlagEnum::NONE };
    constexpr enum_flag flag001{ TestFlagEnum::FLAG0 };
    constexpr enum_flag flag010{ TestFlagEnum::FLAG1 };
    constexpr enum_flag flag100{ TestFlagEnum::FLAG2 };
    constexpr auto flag110 = flag100 | flag010;

    static_assert(!(flag110 & TestFlagEnum::NONE));
    static_assert(!(flag110 & TestFlagEnum::FLAG0));
    static_assert(flag110 & TestFlagEnum::FLAG1);
    static_assert(flag110 & TestFlagEnum::FLAG2);

    constexpr int iflag000 = 0;
    constexpr int iflag001 = 0x0001;
    constexpr int iflag010 = 0x0002;
    constexpr uint iflag100 = 1u << 31;
    constexpr auto iflag110 = iflag100 | iflag010;

    static_assert(enum_flag<TestFlagEnum>{ static_cast<std::uint32_t>(iflag000) } == flag000);
    static_assert(enum_flag<TestFlagEnum>{ static_cast<std::uint32_t>(iflag001) } == flag001);
    static_assert(enum_flag<TestFlagEnum>{ static_cast<std::uint32_t>(iflag010) } == flag010);
    static_assert(enum_flag<TestFlagEnum>{ static_cast<std::uint32_t>(iflag100) } == flag100);
    static_assert(enum_flag<TestFlagEnum>{ static_cast<std::uint32_t>(iflag110) } == flag110);
    static_assert(enum_flag<TestFlagEnum>{ static_cast<std::uint32_t>(iflag110) } != flag000);
    static_assert(enum_flag<TestFlagEnum>{ static_cast<std::uint32_t>(iflag110) } != flag001);
    static_assert(enum_flag<TestFlagEnum>{ static_cast<std::uint32_t>(iflag110) } != flag010);
    static_assert(enum_flag<TestFlagEnum>{ static_cast<std::uint32_t>(iflag110) } != flag100);
    static_assert(enum_flag<TestFlagEnum>{ static_cast<std::uint32_t>(iflag001) } != flag110);
}

} // namespace sl::meta
