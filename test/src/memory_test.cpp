//
// Created by usatiynyan.
//

#include "sl/meta/memory.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

TEST(memory, isTightlyPacked) {
    struct TightlyPackedStruct {
        int i;
        float f;
    };
    static_assert(is_tightly_packed<TightlyPackedStruct>);

    struct NotTightlyPackedStruct {
        int i;
        char c;
    };
    static_assert(!is_tightly_packed<NotTightlyPackedStruct>);
}

TEST(memory, alignedField) {
    struct AlignedFieldAggregate {
        aligned<int, int> i;
        aligned<char, int> c;
        aligned<uint8_t[3], uint32_t> cs;
    };
    static_assert(is_tightly_packed<AlignedFieldAggregate>);
    static_assert(sizeof(AlignedFieldAggregate) == sizeof(int) + sizeof(int) + sizeof(uint32_t));
}

TEST(memory, decorateAllocator) {
    std::size_t alloc_counter = 0;
    std::size_t dealloc_counter = 0;
    auto ai_state = decorate_allocator(
        std::allocator<int>{},
        [&alloc_counter](std::size_t n) { alloc_counter += n; },
        [&dealloc_counter](int*, std::size_t n) { dealloc_counter += n; }
    );

    {
        allocator<int> ai{ ai_state };

        std::vector<int, allocator<int>> vi{ ai };
        ASSERT_EQ(alloc_counter, 0);
        ASSERT_EQ(dealloc_counter, 0);

        vi.push_back(0);
        ASSERT_EQ(alloc_counter, 1);
        ASSERT_EQ(dealloc_counter, 0);

        for (std::size_t i = 0; i < 15; ++i) {
            vi.push_back(0);
        }
        EXPECT_GE(alloc_counter, vi.size());
        EXPECT_GE(dealloc_counter, vi.size() / 2);
    }

    EXPECT_GE(dealloc_counter, 16);
}

} // namespace sl::meta
