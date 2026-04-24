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

TEST(reactive, ctorInv) {
    {
        reactive::state<int> s{};
        auto o = s.make_observer();
        EXPECT_FALSE(o.has_update());
        EXPECT_FALSE(o.peek().has_value());
        EXPECT_FALSE(o.consume().has_value());
    }
    {
        reactive::state<int> s{ 42 };
        auto o = s.make_observer();
        EXPECT_TRUE(o.has_update());
        EXPECT_EQ(o.peek(), 42);
        EXPECT_EQ(o.consume(), 42);
    }
}

TEST(reactive, set) {
    reactive::state<int> s{};
    auto o = s.make_observer();

    EXPECT_FALSE(o.has_update());
    EXPECT_FALSE(o.peek().has_value());

    std::size_t v0 = s.set(42);
    EXPECT_TRUE(o.has_update());
    EXPECT_EQ(o.peek(), 42);

    std::size_t v1 = s.set(100);
    EXPECT_GT(v1, v0);
    EXPECT_TRUE(o.has_update());
    EXPECT_EQ(o.peek(), 100);
}

TEST(reactive, consume) {
    reactive::state<int> s{};
    auto o = s.make_observer();

    s.set(1);
    EXPECT_TRUE(o.has_update());
    EXPECT_EQ(o.consume(), 1);
    EXPECT_FALSE(o.has_update());

    // peek still works after consume
    EXPECT_EQ(o.peek(), 1);

    // consume returns null when no update
    EXPECT_FALSE(o.consume().has_value());

    // new update is visible
    s.set(2);
    EXPECT_TRUE(o.has_update());
    EXPECT_EQ(o.consume(), 2);
    EXPECT_FALSE(o.has_update());
}

TEST(reactive, multipleObservers) {
    reactive::state<int> s{ 0 };
    auto o1 = s.make_observer();
    auto o2 = s.make_observer();

    EXPECT_TRUE(o1.has_update());
    EXPECT_TRUE(o2.has_update());

    EXPECT_EQ(o1.consume(), 0);
    EXPECT_FALSE(o1.has_update());
    EXPECT_TRUE(o2.has_update());

    s.set(42);
    EXPECT_TRUE(o1.has_update());
    EXPECT_TRUE(o2.has_update());

    EXPECT_EQ(o1.consume(), 42);
    EXPECT_EQ(o2.consume(), 42);
    EXPECT_FALSE(o1.has_update());
    EXPECT_FALSE(o2.has_update());
}

TEST(reactive, observerCreatedAfterSet) {
    reactive::state<int> s{};
    s.set(10);
    s.set(20);

    auto o = s.make_observer();
    // observer created after updates should see the current value as an update
    EXPECT_TRUE(o.has_update());
    EXPECT_EQ(o.consume(), 20);
    EXPECT_FALSE(o.has_update());
}

TEST(reactive, all) {
    reactive::state<int> s1{};
    reactive::state<std::string> s2{};

    auto o1 = s1.make_observer();
    auto o2 = s2.make_observer();

    // neither has a value
    EXPECT_FALSE(reactive::all(o1, o2).has_value());

    // only s1 has value
    s1.set(42);
    EXPECT_FALSE(reactive::all(o1, o2).has_value());

    // both have values now
    s2.set("hello");
    {
        auto result = reactive::all(o1, o2);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(std::get<0>(*result), 42);
        EXPECT_EQ(std::get<1>(*result), "hello");
    }

    // after consuming via all, no more updates
    EXPECT_FALSE(reactive::all(o1, o2).has_value());

    // update one - still returns false (both need update)
    s1.set(100);
    EXPECT_FALSE(reactive::all(o1, o2).has_value());

    // update both
    s2.set("world");
    {
        auto result = reactive::all(o1, o2);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(std::get<0>(*result), 100);
        EXPECT_EQ(std::get<1>(*result), "world");
    }
}

TEST(reactive, allSingle) {
    reactive::state<int> s{};
    auto o = s.make_observer();

    EXPECT_FALSE(reactive::all(o).has_value());

    s.set(5);
    {
        auto result = reactive::all(o);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(std::get<0>(*result), 5);
    }

    EXPECT_FALSE(reactive::all(o).has_value());
}

} // namespace sl::meta
