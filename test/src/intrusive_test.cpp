//
// Created by usatiynyan.
//

#include "sl/meta/intrusive.hpp"

#include "sl/meta/fixture.hpp"

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

struct intrusive_int : intrusive_forward_list_node<intrusive_int> {
    int value;
};

TEST(intrusiveNode, forEach) {
    std::vector<intrusive_int> values;
    for (int i = 0; i < 100; ++i) {
        auto& node = values.emplace_back();
        node.value = i;
        node.intrusive_next = nullptr;
    }

    for (std::size_t i = 0; i < 99; ++i) {
        auto& node = values.at(i);
        auto& next = values.at(i + 1);
        node.intrusive_next = &next;
    }

    std::vector<int> for_each_result;
    intrusive_forward_list_node_for_each(&values.front(), [&for_each_result](intrusive_int* x) {
        for_each_result.push_back(x->value);
    });
    ASSERT_EQ(for_each_result.size(), 100);

    for (int i = 0; i < 100; ++i) {
        ASSERT_EQ(for_each_result.at(static_cast<std::size_t>(i)), i);
    }
}

TEST(intrusiveNode, reverse) {
    std::vector<intrusive_int> values;
    for (int i = 0; i < 100; ++i) {
        auto& node = values.emplace_back();
        node.value = i;
        node.intrusive_next = nullptr;
    }

    for (std::size_t i = 0; i < 99; ++i) {
        auto& node = values.at(i);
        auto& next = values.at(i + 1);
        node.intrusive_next = &next;
    }

    intrusive_forward_list_node_reverse(&values.front());

    std::vector<int> for_each_result;
    intrusive_forward_list_node_for_each(&values.back(), [&for_each_result](intrusive_int* x) {
        for_each_result.push_back(x->value);
    });
    ASSERT_EQ(for_each_result.size(), 100);

    for (int i = 0; i < 100; ++i) {
        ASSERT_EQ(for_each_result.at(static_cast<std::size_t>(i)), 99 - i);
    }
}

} // namespace sl::meta
