//
// Created by usatiynyan.
//

#include "sl/meta/intrusive.hpp"

#include "sl/meta/fixture.hpp"
#include "sl/meta/intrusive/list.hpp"

#include <deque>
#include <gtest/gtest.h>

namespace sl::meta {

struct intrusive_forward_lifecycle
    : fixture::lifecycle
    , intrusive_forward_list_node<intrusive_forward_lifecycle> {
    using fixture::lifecycle::lifecycle;
};

TEST(intrusiveForwardList, pushBackPopFront) {
    const auto guard = fixture::lifecycle::make_states_guard();
    constexpr std::size_t expected_count = 10'000;

    intrusive_forward_list<intrusive_forward_lifecycle> list;

    for (std::size_t i = 0; i < expected_count; ++i) {
        list.push_back(new intrusive_forward_lifecycle{ std::to_string(i) });
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

TEST(intrusiveForwardList, append) {
    const auto guard = fixture::lifecycle::make_states_guard();
    constexpr std::size_t expected_count = 10'000;

    intrusive_forward_list<intrusive_forward_lifecycle> list;
    for (std::size_t i = 0; i < expected_count / 2; ++i) {
        list.push_back(new intrusive_forward_lifecycle{ std::to_string(i) });
    }

    intrusive_forward_list<intrusive_forward_lifecycle> another_list;
    for (std::size_t i = expected_count / 2; i < expected_count; ++i) {
        another_list.push_back(new intrusive_forward_lifecycle{ std::to_string(i) });
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

TEST(intrusiveForwardList, swap) {
    const auto guard = fixture::lifecycle::make_states_guard();
    constexpr std::size_t expected_count = 10'000;

    {
        intrusive_forward_list<intrusive_forward_lifecycle> list;
        for (std::size_t i = 0; i < expected_count; ++i) {
            list.push_back(new intrusive_forward_lifecycle{ std::to_string(i) });
        }

        intrusive_forward_list<intrusive_forward_lifecycle> another_list;

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

struct intrusive_forward_int : intrusive_forward_list_node<intrusive_forward_int> {
    int value;
};

TEST(intrusiveForwardListNode, forEach) {
    std::vector<intrusive_forward_int> values;
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
    intrusive_forward_list_node_for_each(&values.front(), [&for_each_result](intrusive_forward_int* x) {
        for_each_result.push_back(x->value);
    });
    ASSERT_EQ(for_each_result.size(), 100);

    for (int i = 0; i < 100; ++i) {
        ASSERT_EQ(for_each_result.at(static_cast<std::size_t>(i)), i);
    }
}

TEST(intrusiveForwardListNode, reverse) {
    std::vector<intrusive_forward_int> values;
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
    intrusive_forward_list_node_for_each(&values.back(), [&for_each_result](intrusive_forward_int* x) {
        for_each_result.push_back(x->value);
    });
    ASSERT_EQ(for_each_result.size(), 100);

    for (int i = 0; i < 100; ++i) {
        ASSERT_EQ(for_each_result.at(static_cast<std::size_t>(i)), 99 - i);
    }
}

struct intrusive_lifecycle
    : fixture::lifecycle
    , intrusive_list_node<intrusive_lifecycle> {
    using fixture::lifecycle::lifecycle;
};

TEST(intrusiveList, pushBackPopBack) {
    const auto guard = fixture::lifecycle::make_states_guard();

    {
        intrusive_list<intrusive_lifecycle> list;
        ASSERT_TRUE(list.empty());
        ASSERT_EQ(list.size(), 0);
        ASSERT_EQ(list.back(), nullptr);
        ASSERT_EQ(list.front(), nullptr);

        intrusive_lifecycle back_node_0{ "back_node_0" };
        list.push_back(&back_node_0);
        EXPECT_TRUE(!list.empty());
        EXPECT_EQ(list.size(), 1);
        EXPECT_EQ(list.back(), &back_node_0);
        EXPECT_EQ(list.front(), &back_node_0);
        EXPECT_EQ(back_node_0.intrusive_next, nullptr);
        EXPECT_EQ(back_node_0.intrusive_prev, nullptr);

        intrusive_lifecycle back_node_1{ "back_node_1" };
        list.push_back(&back_node_1);
        EXPECT_EQ(list.size(), 2);
        EXPECT_EQ(list.back(), &back_node_1);
        EXPECT_EQ(list.front(), &back_node_0);
        EXPECT_EQ(back_node_1.intrusive_next, nullptr);
        EXPECT_EQ(back_node_1.intrusive_prev, &back_node_0);
        EXPECT_EQ(back_node_0.intrusive_next, &back_node_1);
        EXPECT_EQ(back_node_0.intrusive_prev, nullptr);

        intrusive_lifecycle back_node_2{ "back_node_2" };
        list.push_back(&back_node_2);
        EXPECT_EQ(list.size(), 3);
        EXPECT_EQ(list.back(), &back_node_2);
        EXPECT_EQ(list.front(), &back_node_0);
        EXPECT_EQ(back_node_2.intrusive_next, nullptr);
        EXPECT_EQ(back_node_2.intrusive_prev, &back_node_1);
        EXPECT_EQ(back_node_1.intrusive_next, &back_node_2);
        EXPECT_EQ(back_node_1.intrusive_prev, &back_node_0);
        EXPECT_EQ(back_node_0.intrusive_next, &back_node_1);
        EXPECT_EQ(back_node_0.intrusive_prev, nullptr);

        EXPECT_EQ(list.pop_back(), &back_node_2);
        EXPECT_TRUE(!list.empty() && list.size() == 2);
        EXPECT_EQ(list.pop_back(), &back_node_1);
        EXPECT_TRUE(!list.empty() && list.size() == 1);
        EXPECT_EQ(list.pop_back(), &back_node_0);
        EXPECT_TRUE(list.empty() && list.size() == 0);
        EXPECT_EQ(list.pop_back(), nullptr);
        EXPECT_TRUE(list.empty() && list.size() == 0);
    }

    const std::vector<fixture::lifecycle::state> expected_states{
        fixture::lifecycle::state::constructed,
        fixture::lifecycle::state::destructed,
    };
    EXPECT_EQ(fixture::lifecycle::states["back_node_0"], expected_states);
    EXPECT_EQ(fixture::lifecycle::states["back_node_1"], expected_states);
    EXPECT_EQ(fixture::lifecycle::states["back_node_2"], expected_states);
}

TEST(intrusiveList, pushFrontPopFront) {
    const auto guard = fixture::lifecycle::make_states_guard();

    intrusive_list<intrusive_lifecycle> list;

    {
        intrusive_lifecycle front_node_0{ "front_node_0" };
        list.push_front(&front_node_0);
        EXPECT_TRUE(!list.empty());
        EXPECT_EQ(list.size(), 1);
        EXPECT_EQ(list.back(), &front_node_0);
        EXPECT_EQ(list.front(), &front_node_0);
        EXPECT_EQ(front_node_0.intrusive_prev, nullptr);
        EXPECT_EQ(front_node_0.intrusive_next, nullptr);

        intrusive_lifecycle front_node_1{ "front_node_1" };
        list.push_front(&front_node_1);
        EXPECT_EQ(list.size(), 2);
        EXPECT_EQ(list.back(), &front_node_0);
        EXPECT_EQ(list.front(), &front_node_1);
        EXPECT_EQ(front_node_1.intrusive_prev, nullptr);
        EXPECT_EQ(front_node_1.intrusive_next, &front_node_0);
        EXPECT_EQ(front_node_0.intrusive_prev, &front_node_1);
        EXPECT_EQ(front_node_0.intrusive_next, nullptr);

        intrusive_lifecycle front_node_2{ "front_node_2" };
        list.push_front(&front_node_2);
        EXPECT_EQ(list.size(), 3);
        EXPECT_EQ(list.back(), &front_node_0);
        EXPECT_EQ(list.front(), &front_node_2);
        EXPECT_EQ(front_node_2.intrusive_prev, nullptr);
        EXPECT_EQ(front_node_2.intrusive_next, &front_node_1);
        EXPECT_EQ(front_node_1.intrusive_prev, &front_node_2);
        EXPECT_EQ(front_node_1.intrusive_next, &front_node_0);
        EXPECT_EQ(front_node_0.intrusive_prev, &front_node_1);
        EXPECT_EQ(front_node_0.intrusive_next, nullptr);

        EXPECT_EQ(list.pop_front(), &front_node_2);
        EXPECT_TRUE(!list.empty() && list.size() == 2);
        EXPECT_EQ(list.pop_front(), &front_node_1);
        EXPECT_TRUE(!list.empty() && list.size() == 1);
        EXPECT_EQ(list.pop_front(), &front_node_0);
        EXPECT_TRUE(list.empty() && list.size() == 0);
        EXPECT_EQ(list.pop_front(), nullptr);
        EXPECT_TRUE(list.empty() && list.size() == 0);
    }

    const std::vector<fixture::lifecycle::state> expected_states{
        fixture::lifecycle::state::constructed,
        fixture::lifecycle::state::destructed,
    };
    EXPECT_EQ(fixture::lifecycle::states["front_node_0"], expected_states);
    EXPECT_EQ(fixture::lifecycle::states["front_node_1"], expected_states);
    EXPECT_EQ(fixture::lifecycle::states["front_node_2"], expected_states);
}

TEST(intrusiveList, forEach) {
    const auto guard = fixture::lifecycle::make_states_guard();

    constexpr std::size_t expected_size = 100;
    intrusive_list<intrusive_lifecycle> list;

    std::vector<intrusive_lifecycle> buffer;
    buffer.reserve(expected_size);

    std::deque<std::string> expected_ids;
    for (std::size_t i = 0; i < expected_size; ++i) {
        const auto id = std::to_string(i);
        auto& node = buffer.emplace_back(id);
        if (i % 2 == 0) {
            list.push_back(&node);
            expected_ids.push_back(id);
        } else {
            list.push_front(&node);
            expected_ids.push_front(id);
        }
    }

    {
        auto expected_it = expected_ids.begin();
        for (auto& node : list) {
            EXPECT_EQ(node.id, *expected_it++);
        }
        EXPECT_EQ(expected_it, expected_ids.end());
    }
}

TEST(intrusiveList, tryErase) {
    const auto guard = fixture::lifecycle::make_states_guard();

    constexpr std::size_t expected_size = 100;
    intrusive_list<intrusive_lifecycle> list;

    std::vector<intrusive_lifecycle> buffer;
    buffer.reserve(expected_size);

    for (std::size_t i = 0; i < expected_size; ++i) {
        const auto id = std::to_string(i);
        auto& node = buffer.emplace_back(id);
        list.push_back(&node);
    }

    {
        std::size_t i = 0;
        for (auto& node : list) {
            ASSERT_EQ(node.id, std::to_string(i++));
        }
        ASSERT_EQ(i, expected_size);
    }

    {
        auto& node = buffer.at(42);
        auto* prev_node = node.intrusive_prev;
        auto* next_node = node.intrusive_next;
        const auto result = list.erase(&node);
        EXPECT_EQ(node.intrusive_prev, nullptr);
        EXPECT_EQ(node.intrusive_next, nullptr);
        EXPECT_EQ(result.prev, prev_node);
        EXPECT_EQ(result.next, next_node);
        EXPECT_EQ(result.prev->downcast()->id, std::to_string(41));
        EXPECT_EQ(result.next->downcast()->id, std::to_string(43));
        EXPECT_EQ(list.size(), expected_size - 1);
    }

    {
        auto& node = buffer.front();
        EXPECT_EQ(node.id, std::to_string(0));
        EXPECT_EQ(node.intrusive_prev, nullptr);
        auto* next_node = node.intrusive_next;
        const auto result = list.erase(&node);
        EXPECT_EQ(node.intrusive_prev, nullptr);
        EXPECT_EQ(node.intrusive_next, nullptr);
        EXPECT_EQ(result.prev, nullptr);
        EXPECT_EQ(result.next, next_node);
        EXPECT_EQ(result.next->downcast()->id, std::to_string(1));
        EXPECT_EQ(list.size(), expected_size - 2);
    }

    {
        auto& node = buffer.back();
        EXPECT_EQ(node.id, std::to_string(99));
        auto* prev_node = node.intrusive_prev;
        EXPECT_EQ(node.intrusive_next, nullptr);
        const auto result = list.erase(&node);
        EXPECT_EQ(node.intrusive_prev, nullptr);
        EXPECT_EQ(node.intrusive_next, nullptr);
        EXPECT_EQ(result.prev, prev_node);
        EXPECT_EQ(result.next, nullptr);
        EXPECT_EQ(result.prev->downcast()->id, std::to_string(98));
        EXPECT_EQ(list.size(), expected_size - 3);
    }

    while (list.size() > 1) {
        if (list.size() % 2 == 0) {
            std::ignore = list.erase(list.back());
        } else {
            std::ignore = list.erase(list.front());
        }
    }

    {
        EXPECT_EQ(list.back(), list.front());
        auto* node = list.back();
        EXPECT_EQ(node->intrusive_prev, nullptr);
        EXPECT_EQ(node->intrusive_next, nullptr);
        const auto result = list.erase(node);
        EXPECT_EQ(result.prev, nullptr);
        EXPECT_EQ(result.next, nullptr);
        EXPECT_EQ(list.back(), nullptr);
        EXPECT_EQ(list.front(), nullptr);
        EXPECT_EQ(list.size(), 0);
        EXPECT_TRUE(list.empty());
    }
}

} // namespace sl::meta
