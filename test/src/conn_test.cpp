//
// Created by usatiynyan.
//

#include "sl/meta/conn.hpp"

#include <gtest/gtest.h>


namespace sl::meta {

TEST(conn, oneUnit) {
    signal<unit> signal;
    size_t counter = 0;
    auto conn = signal.connect([&](unit) { ++counter; });
    signal({});
    ASSERT_EQ(counter, 1);
    signal.disconnect(std::move(conn));
    signal({});
    ASSERT_EQ(counter, 1);
}

} // namespace sl::meta
