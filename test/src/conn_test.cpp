//
// Created by usatiynyan.
//

#include "sl/meta/conn.hpp"
#include "sl/meta/type/identity.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

TEST(conn, oneUnit) {
    signal<> signal;
    size_t counter = 0;
    auto conn = signal.connect([&] { ++counter; });
    signal();
    ASSERT_EQ(counter, 1);
    signal.disconnect(std::move(conn));
    signal();
    ASSERT_EQ(counter, 1);
}

TEST(conn, manyUnit) {
    signal<> signal;
    size_t counter1 = 0;
    auto conn1 = signal.connect([&]() { ++counter1; });
    size_t counter2 = 0;
    auto conn2 = signal.connect([&]() { ++counter2; });
    signal();
    EXPECT_EQ(counter1, 1);
    EXPECT_EQ(counter2, 1);
    signal.disconnect(std::move(conn1));
    signal();
    EXPECT_EQ(counter1, 1);
    EXPECT_EQ(counter2, 2);
    signal.disconnect(std::move(conn2));
}

TEST(conn, oneValue) {
    signal<std::string> signal;
    std::string result;
    auto conn = signal.connect([&](std::string_view s) { result = s; });
    signal("hello");
    ASSERT_EQ(result, "hello");
    signal("world");
    ASSERT_EQ(result, "world");
    signal.disconnect(std::move(conn));
    signal("bye");
    ASSERT_EQ(result, "world");
}

TEST(conn, manyValue) {
    signal<std::string> signal;
    std::string result1;
    auto conn1 = signal.connect([&](std::string_view s) { result1 = s; });
    std::string result2;
    auto conn2 = signal.connect([&](std::string_view s) { result2 = s; });
    signal("hello");
    ASSERT_EQ(result1, "hello");
    ASSERT_EQ(result2, "hello");
    signal.disconnect(std::move(conn1));
    signal("world");
    ASSERT_EQ(result1, "hello");
    ASSERT_EQ(result2, "world");
    signal.disconnect(std::move(conn2));
}

TEST(conn, scope) {
    signal<> signal;
    size_t counter = 0;
    {
        scoped_conn<> scoped_conn{ signal, [&] { ++counter; } };
        signal();
        ASSERT_EQ(counter, 1);
    }
    signal();
    ASSERT_EQ(counter, 1);
}

TEST(conn, property) {
    property<std::string> string_property;
    size_t counter = 0;
    {
        std::string_view from_changed;
        scoped_conn scoped_conn{
            string_property.changed(),
            [&](std::string_view x) {
                from_changed = x;
                ++counter;
            },
        };
        string_property.set("hello");
        EXPECT_EQ(counter, 1);
        EXPECT_EQ(from_changed, "hello");

        string_property.set("world");
        EXPECT_EQ(counter, 2);
        EXPECT_EQ(from_changed, "world");

        string_property.set("world");
        EXPECT_EQ(counter, 2);
        EXPECT_EQ(from_changed, "world");
    }
    string_property.set("!");
    EXPECT_EQ(counter, 2);
}

TEST(conn, dirty) {
    dirty<std::string> dirty_string{ "hello" };
    EXPECT_EQ(dirty_string.get(), "hello");
    {
        const auto changed_value = dirty_string.release();
        ASSERT_TRUE(changed_value.has_value());
        EXPECT_EQ(changed_value.value(), "hello");
    }
    {
        ASSERT_FALSE(dirty_string.release().has_value());
        dirty_string.set_dirty(true);
        const auto changed_value = dirty_string.release();
        ASSERT_TRUE(changed_value.has_value());
        EXPECT_EQ(changed_value.value(), "hello");
    }

    dirty<std::string> dirty_string_empty;
    EXPECT_EQ(dirty_string_empty.get(), tl::nullopt);
    EXPECT_FALSE(dirty_string_empty.release().has_value());

    dirty_string_empty.set("hello");
    EXPECT_EQ(dirty_string_empty.get(), "hello");
    {
        const auto changed_value = dirty_string_empty.release();
        ASSERT_TRUE(changed_value.has_value());
        EXPECT_EQ(changed_value.value(), "hello");
    }

    dirty_string_empty.set("hello");
    EXPECT_EQ(dirty_string_empty.get(), "hello");
    EXPECT_TRUE(dirty_string_empty.release().has_value()) << "mark as changed even if value is the same";

    dirty_string_empty.set("world");
    EXPECT_EQ(dirty_string_empty.get(), "world");
    {
        const auto changed_value = dirty_string_empty.release();
        ASSERT_TRUE(changed_value.has_value());
        EXPECT_EQ(changed_value.value(), "world");
    }
}

} // namespace sl::meta
