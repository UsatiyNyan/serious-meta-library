//
// Created by usatiynyan on 8/6/23.
//

#include "sl/meta/hash.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

TEST(hash, fnv1a) {
    using std::literals::operator""sv;

    static_assert(fnv1a<std::uint32_t>(""sv) == 0x811c9dc5, "Test failed!");
    static_assert(fnv1a<std::uint32_t>("a"sv) == 0xe40c292c, "Test failed!");
    static_assert(fnv1a<std::uint32_t>("foo"sv) == 0xa9f37ed7, "Test failed!");
    static_assert(fnv1a<std::uint32_t>("foobar"sv) == 0xbf9cf968, "Test failed!");
    static_assert(fnv1a<std::uint32_t>("0123456789ABCDEF"sv) == 0xe9cb8efd, "Test failed!");
}

} // namespace sl::meta
