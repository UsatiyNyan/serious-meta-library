//
// Created by usatiynyan.
//

#include "sl/meta/func.hpp"

#include <gtest/gtest.h>
#include <tl/expected.hpp>

namespace sl::meta {

TEST(func, pipelineEmpty) {
    constexpr pipeline p;
    EXPECT_EQ(p(123), 123);
}

TEST(func, pipeline) {
    constexpr auto p = pipeline{} //
                           .then(SL_META_LIFT(std::to_string))
                           .then([](auto x) { return x + "abc"; })
                           .then([](auto x) { return x + x; });
    EXPECT_EQ(p(123), "123abc123abc");
    EXPECT_EQ(p(10.1f), "10.100000abc10.100000abc");
}

TEST(func, undefined) {
    tl::expected<unit, undefined> never_error;
    never_error = unit{};
}

} // namespace sl::meta
