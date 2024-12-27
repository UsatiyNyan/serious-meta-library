//
// Created by usatiynyan.
//

#include "sl/meta/type/list.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

TEST(type, areSame) {
    static_assert(type::are_same_v<int>);
    static_assert(type::are_same_v<int, int, int>);
    static_assert(!type::are_same_v<short, int>);
    static_assert(!type::are_same_v<int, int, char, int>);
    static_assert(!type::are_same_v<int, std::string, char>);
}

} // namespace sl::meta
