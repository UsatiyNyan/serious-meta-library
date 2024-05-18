//
// Created by usatiynyan on 8/6/23.
//

#include "sl/meta/hash.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

TEST(hash, fnv1a) {
    static_assert(fnv1a<std::uint32_t>("") == 0x811c9dc5, "Test failed!");
    static_assert(fnv1a<std::uint32_t>("a") == 0xe40c292c, "Test failed!");
    static_assert(fnv1a<std::uint32_t>("foo") == 0xa9f37ed7, "Test failed!");
    static_assert(fnv1a<std::uint32_t>("foobar") == 0xbf9cf968, "Test failed!");
    static_assert(fnv1a<std::uint32_t>("0123456789ABCDEF") == 0xe9cb8efd, "Test failed!");
}

} // namespace sl::meta
