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

TEST(staticString, asTemplateArg) { static_assert(StringHolder<chars>::value.string_view() == std::string_view{ chars }); }

} // namespace sl::meta
