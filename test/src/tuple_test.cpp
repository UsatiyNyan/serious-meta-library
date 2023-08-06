//
// Created by usatiynyan on 7/30/23.
//

#include "sl/meta/tuple.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

class for_each_fixture : public ::testing::Test {
protected:
    int i_field_ = 10;
    std::string s_field_ = "oraoraoraoraora";
    std::tuple<int, std::string> tuple_{ i_field_, s_field_ };
};

TEST_F(for_each_fixture, cref) {
    const auto& tuple_cref = tuple_;
    size_t i = 0;
    for_each(
        [this, &i](const auto& field) {
            ASSERT_EQ(std::get<std::decay_t<decltype(field)>>(tuple_), field);
            ++i;
        },
        tuple_cref
    );
    ASSERT_EQ(i, std::tuple_size_v<std::decay_t<decltype(tuple_)>>);
}

TEST_F(for_each_fixture, ref) {
    for_each([](auto& field) { field = field + field; }, tuple_);
    ASSERT_EQ(std::get<0>(tuple_), i_field_ + i_field_);
    ASSERT_EQ(std::get<1>(tuple_), s_field_ + s_field_);
}

TEST(tuple, applyAsTuple) {
    struct TestStruct {
        int i;
        std::string s;
    };

    const auto result = apply([](int an_i, std::string an_s) {
        return std::to_string(an_i) + std::move(an_s);
    }, TestStruct{100, "aaa"});

    ASSERT_EQ(result, "100aaa");
}

} // namespace sl::meta
