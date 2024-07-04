//
// Created by usatiynyan.
//

#include "fixture/lifecycle.hpp"
#include "sl/meta/intrusive.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

template <typename T>
struct intrusive_value
    : T
    , intrusive_forward_list_node<intrusive_value<T>> {
    using T::T;
};

TEST(intrusive, list) {
    const auto guard = fixture::lifecycle::make_states_guard();

    intrusive_forward_list<intrusive_value<fixture::lifecycle>> list;

    for (int i = 0; i < 10; ++i) {
        auto* node = new intrusive_value<fixture::lifecycle>(std::to_string(i));
        list.push_back(node);
    }

    ASSERT_FALSE(list.empty());
    ASSERT_EQ(list.size(), 10);

    {
        int i = 0;
        for (auto& item : list) {
            EXPECT_EQ(item.id, std::to_string(i++));
        }
        EXPECT_EQ(i, 10);
    }

    while (auto* node = list.pop_front()) {
        delete node;
    }

    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);

    for (int i = 0; i < 10; ++i) {
        const std::vector<fixture::lifecycle::state> expected{
            fixture::lifecycle::state::constructed,
            fixture::lifecycle::state::destructed,
        };
        EXPECT_EQ(fixture::lifecycle::states[std::to_string(i)], expected);
    }
}


TEST(intrusive, listAppend) {
    const auto guard = fixture::lifecycle::make_states_guard();

    intrusive_forward_list<intrusive_value<fixture::lifecycle>> list;
    for (int i = 0; i < 5; ++i) {
        auto* node = new intrusive_value<fixture::lifecycle>(std::to_string(i));
        list.push_back(node);
    }

    intrusive_forward_list<intrusive_value<fixture::lifecycle>> another_list;
    for (int i = 5; i < 10; ++i) {
        auto* node = new intrusive_value<fixture::lifecycle>(std::to_string(i));
        another_list.push_back(node);
    }

    list.append(std::move(another_list));

    ASSERT_FALSE(list.empty());
    ASSERT_EQ(list.size(), 10);

    EXPECT_TRUE(another_list.empty());
    EXPECT_EQ(another_list.size(), 0);

    {
        int i = 0;
        for (auto& item : list) {
            EXPECT_EQ(item.id, std::to_string(i++));
        }
        EXPECT_EQ(i, 10);
    }

    while (auto* node = list.pop_front()) {
        delete node;
    }

    for (int i = 0; i < 10; ++i) {
        const std::vector<fixture::lifecycle::state> expected{
            fixture::lifecycle::state::constructed,
            fixture::lifecycle::state::destructed,
        };
        EXPECT_EQ(fixture::lifecycle::states[std::to_string(i)], expected);
    }
}

TEST(intrusive, listSwap) {
    const auto guard = fixture::lifecycle::make_states_guard();

    {
        intrusive_forward_list<intrusive_value<fixture::lifecycle>> list;
        for (int i = 0; i < 10; ++i) {
            auto* node = new intrusive_value<fixture::lifecycle>(std::to_string(i));
            list.push_back(node);
        }

        intrusive_forward_list<intrusive_value<fixture::lifecycle>> another_list;

        std::swap(list, another_list);

        ASSERT_TRUE(list.empty());
        ASSERT_FALSE(another_list.empty());

        {
            int i = 0;
            for (auto& item : another_list) {
                EXPECT_EQ(item.id, std::to_string(i++));
            }
            EXPECT_EQ(i, 10);
        }

        while (auto* node = another_list.pop_front()) {
            delete node;
        }
    }

    for (int i = 0; i < 10; ++i) {
        const std::vector<fixture::lifecycle::state> expected{
            fixture::lifecycle::state::constructed,
            fixture::lifecycle::state::destructed,
        };
        EXPECT_EQ(fixture::lifecycle::states[std::to_string(i)], expected);
    }
}

} // namespace sl::meta
