//
// Created by usatiynyan on 7/28/23.
//

#include "sl/meta/string.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

constexpr char chars[] = "oraoraoraoraoraoraoraoraora";

TEST(staticString, create) { static_assert(static_string{ chars }.string_view() == std::string_view{ chars }); }

template <static_string value_>
struct StringHolder {
    static constexpr static_string value = value_;
};

TEST(staticString, asTemplateArg) {
    static_assert(StringHolder<chars>::value.string_view() == std::string_view{ chars });
}

TEST(hashStringView, literal) {
    static_assert("ora"_hsv.hash() == 0x19fb8e1921bfa2bf);
    static_assert("ora"_hsv == "ora"_hsv);
    static_assert("oraora"_hsv != "ora"_hsv);
}

} // namespace sl::meta
