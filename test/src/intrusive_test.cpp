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
    constexpr std::size_t expected_count = 10'000;

    intrusive_forward_list<intrusive_value<fixture::lifecycle>> list;

    for (std::size_t i = 0; i < expected_count; ++i) {
        auto* node = new intrusive_value<fixture::lifecycle>(std::to_string(i));
        list.push_back(node);
    }

    ASSERT_FALSE(list.empty());
    ASSERT_EQ(list.size(), expected_count);

    {
        std::size_t i = 0;
        for (auto& item : list) {
            EXPECT_EQ(item.id, std::to_string(i++));
        }
        EXPECT_EQ(i, expected_count);
    }

    while (auto* node = list.pop_front()) {
        delete node;
    }

    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);

    for (std::size_t i = 0; i < expected_count; ++i) {
        const std::vector<fixture::lifecycle::state> expected_states{
            fixture::lifecycle::state::constructed,
            fixture::lifecycle::state::destructed,
        };
        EXPECT_EQ(fixture::lifecycle::states[std::to_string(i)], expected_states);
    }
}


TEST(intrusive, listAppend) {
    const auto guard = fixture::lifecycle::make_states_guard();
    constexpr std::size_t expected_count = 10'000;

    intrusive_forward_list<intrusive_value<fixture::lifecycle>> list;
    for (std::size_t i = 0; i < expected_count / 2; ++i) {
        auto* node = new intrusive_value<fixture::lifecycle>(std::to_string(i));
        list.push_back(node);
    }

    intrusive_forward_list<intrusive_value<fixture::lifecycle>> another_list;
    for (std::size_t i = expected_count / 2; i < expected_count; ++i) {
        auto* node = new intrusive_value<fixture::lifecycle>(std::to_string(i));
        another_list.push_back(node);
    }

    list.append(std::move(another_list));

    ASSERT_FALSE(list.empty());
    ASSERT_EQ(list.size(), expected_count);

    EXPECT_TRUE(another_list.empty());
    EXPECT_EQ(another_list.size(), 0);

    {
        std::size_t i = 0;
        for (auto it = list.begin(); it != list.end(); ++it) {
            auto& item = *it;
            EXPECT_EQ(item.id, std::to_string(i++));
            delete &item;
        }
        EXPECT_EQ(i, expected_count);
    }

    for (std::size_t i = 0; i < expected_count; ++i) {
        const std::vector<fixture::lifecycle::state> expected_states{
            fixture::lifecycle::state::constructed,
            fixture::lifecycle::state::destructed,
        };
        EXPECT_EQ(fixture::lifecycle::states[std::to_string(i)], expected_states);
    }
}

TEST(intrusive, listSwap) {
    const auto guard = fixture::lifecycle::make_states_guard();
    constexpr std::size_t expected_count = 10'000;

    {
        intrusive_forward_list<intrusive_value<fixture::lifecycle>> list;
        for (std::size_t i = 0; i < expected_count; ++i) {
            auto* node = new intrusive_value<fixture::lifecycle>(std::to_string(i));
            list.push_back(node);
        }

        intrusive_forward_list<intrusive_value<fixture::lifecycle>> another_list;

        std::swap(list, another_list);

        ASSERT_TRUE(list.empty());
        ASSERT_FALSE(another_list.empty());

        {
            std::size_t i = 0;
            for (auto it = another_list.begin(); it != list.end(); it++) {
                auto& item = *it;
                EXPECT_EQ(item.id, std::to_string(i++));
                delete &item;
            }
            EXPECT_EQ(i, expected_count);
        }
    }

    for (std::size_t i = 0; i < expected_count; ++i) {
        const std::vector<fixture::lifecycle::state> expected_states{
            fixture::lifecycle::state::constructed,
            fixture::lifecycle::state::destructed,
        };
        EXPECT_EQ(fixture::lifecycle::states[std::to_string(i)], expected_states);
    }
}

} // namespace sl::meta
