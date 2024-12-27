//
// Created by usatiynyan.
//

#include "sl/meta/type/list.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

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

TEST(type, areSame) {
    static_assert(type::are_same_v<int>);
    static_assert(type::are_same_v<int, int, int>);
    static_assert(!type::are_same_v<short, int>);
    static_assert(!type::are_same_v<int, int, char, int>);
    static_assert(!type::are_same_v<int, std::string, char>);
}

} // namespace sl::meta
