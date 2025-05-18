//
// Created by usatiynyan.
//

#include "sl/meta/type.hpp"
#include <tl/expected.hpp>

#include <gtest/gtest.h>
#include <type_traits>

namespace sl::meta {

template <typename... Ts>
struct head_test {
    using type = type::head_t<Ts...>;
};

TEST(type, head) {
    static_assert(std::is_same_v<int, type::head_t<int>>);
    static_assert(std::is_same_v<int, type::head_t<int, char>>);
    static_assert(std::is_same_v<char, type::head_t<char, int, char>>);
}

TEST(type, at) {
    static_assert(std::is_same_v<int, type::at_t<0, int>>);
    static_assert(std::is_same_v<void, type::at_t<3, int, char, std::string, void>>);
    static_assert(std::is_same_v<std::string, type::at_t<2, int, char, std::string, void>>);
}

TEST(type, repeat) {
    static_assert(std::is_same_v<std::tuple<>, type::repeat_t<meta::undefined, 0>>);
    static_assert(std::is_same_v<std::tuple<int, int, int>, type::repeat_t<int, 3>>);
    static_assert(std::is_same_v<std::tuple<std::string>, type::repeat_t<std::string, 1>>);
}

TEST(type, areSame) {
    static_assert(type::are_same_v<int>);
    static_assert(type::are_same_v<int, int, int>);
    static_assert(!type::are_same_v<short, int>);
    static_assert(!type::are_same_v<int, int, char, int>);
    static_assert(!type::are_same_v<int, std::string, char>);
}

TEST(type, undefined) {
    static_assert(!std::is_constructible_v<undefined>);

    using expected_type = tl::expected<std::string, undefined>;
    static_assert(std::constructible_from<expected_type, std::string>);

    expected_type expected{ "hehe" };
    // expected.emplace(undefined{}); // compilation error :3
    // expected = tl::unexpected(undefined{}); // compilation error :3
}

constexpr char chars[] = "oraoraoraoraoraoraoraoraora";

TEST(staticString, create) { static_assert(static_string{ chars }.string_view() == std::string_view{ chars }); }

template <static_string value_>
struct StringHolder {
    static constexpr static_string value = value_;
};

TEST(staticString, asTemplateArg) {
    static_assert(StringHolder<chars>::value.string_view() == std::string_view{ chars });
}

} // namespace sl::meta
