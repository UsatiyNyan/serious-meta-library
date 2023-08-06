//
// Created by usatiynyan on 7/28/23.
//

#include "sl/meta/string.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

constexpr char chars[] = "oraoraoraoraoraoraoraoraora";

TEST(string, create) {
    static_assert(string{chars}.chars == std::string_view{chars});
}

template <string value_>
struct StringHolder {
    static constexpr string value = value_;
};

TEST(string, asTemplateArg) {
    static_assert(StringHolder<chars>::value.chars == std::string_view{chars});
}

} // namespace sl::meta
