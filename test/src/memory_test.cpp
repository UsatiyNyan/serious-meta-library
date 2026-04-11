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

// ============== small_vector tests ==============

TEST(small_vector, emplace_back_returns_reference) {
    small_vector<int, 4> sv;
    int& ref = sv.emplace_back(42);
    EXPECT_EQ(ref, 42);
    ref = 100;
    EXPECT_EQ(sv.at_unchecked(0), 100);
}

TEST(small_vector, basic_operations) {
    small_vector<int, 4> sv;
    sv.push_back(1);
    sv.push_back(2);
    sv.push_back(3);
    EXPECT_EQ(sv.size(), 3);
    EXPECT_EQ(sv.at_unchecked(0), 1);
    EXPECT_EQ(sv.at_unchecked(2), 3);

    auto popped = sv.pop_back();
    ASSERT_TRUE(popped.has_value());
    EXPECT_EQ(popped.value(), 3);
    EXPECT_EQ(sv.size(), 2);
}

TEST(small_vector, growth_beyond_static) {
    small_vector<int, 2> sv;
    sv.push_back(1);
    sv.push_back(2);
    EXPECT_TRUE(sv.is_static());
    sv.push_back(3); // triggers growth
    EXPECT_FALSE(sv.is_static());
    EXPECT_EQ(sv.size(), 3);
    EXPECT_EQ(sv.at_unchecked(0), 1);
    EXPECT_EQ(sv.at_unchecked(2), 3);
}

struct DestructorCounter {
    static inline int count = 0;
    int value;
    explicit DestructorCounter(int v) : value(v) {}
    DestructorCounter(DestructorCounter&& other) noexcept : value(other.value) {}
    ~DestructorCounter() { ++count; }
};

TEST(small_vector, destructor_calls_element_destructors_on_heap) {
    DestructorCounter::count = 0;
    {
        small_vector<DestructorCounter, 2> sv;
        sv.emplace_back(1);
        sv.emplace_back(2);
        sv.emplace_back(3); // triggers growth to heap
        sv.emplace_back(4);
        EXPECT_EQ(sv.size(), 4);
        // After growth, 2 elements were moved and destroyed from static storage
        // So count should be 2 at this point
    }
    // All 4 elements should be destroyed: 2 from move during growth + 4 final = 6
    EXPECT_EQ(DestructorCounter::count, 6);
}

struct AllocationTracker {
    static inline int live_count = 0;
    int* data;
    explicit AllocationTracker(int v) : data(new int(v)) { ++live_count; }
    AllocationTracker(AllocationTracker&& other) noexcept : data(other.data) {
        other.data = nullptr;
        ++live_count;
    }
    ~AllocationTracker() {
        delete data;
        --live_count;
    }
};

TEST(small_vector, no_memory_leak_on_multiple_growths) {
    AllocationTracker::live_count = 0;
    {
        small_vector<AllocationTracker, 1> sv;
        sv.emplace_back(1); // size=1, capacity=1 (static)
        sv.emplace_back(2); // grow to capacity=2 (heap)
        sv.emplace_back(3); // grow to capacity=4 (heap, old heap should be freed)
        sv.emplace_back(4);
        sv.emplace_back(5); // grow to capacity=8 (heap, old heap should be freed)
        EXPECT_EQ(sv.size(), 5);
    }
    // All allocations should be cleaned up
    EXPECT_EQ(AllocationTracker::live_count, 0);
}

// BUG: static_data_ is not aligned for T, heap allocation also unaligned
struct alignas(128) OverAligned128 {
    int value;
    explicit OverAligned128(int v) : value(v) {}
};

TEST(small_vector, alignment_static_storage) {
    small_vector<OverAligned128, 2> sv;
    sv.emplace_back(42);
    // Check that address is properly aligned to 128 bytes
    auto addr = reinterpret_cast<std::uintptr_t>(&sv.at_unchecked(0));
    EXPECT_EQ(addr % 128, 0) << "Static storage not properly aligned for alignas(128) type";
}

TEST(small_vector, alignment_heap_storage) {
    small_vector<OverAligned128, 1> sv;
    sv.emplace_back(1);
    sv.emplace_back(2); // triggers growth to heap
    // Check that heap address is properly aligned to 128 bytes
    auto addr = reinterpret_cast<std::uintptr_t>(&sv.at_unchecked(0));
    EXPECT_EQ(addr % 128, 0) << "Heap storage not properly aligned for alignas(128) type";
}

TEST(small_vector, begin_end) {
    small_vector<int, 4> sv;
    sv.push_back(1);
    sv.push_back(2);
    sv.push_back(3);

    int sum = 0;
    for (int x : sv) {
        sum += x;
    }
    EXPECT_EQ(sum, 6);
}

TEST(small_vector, move_constructor_static) {
    small_vector<int, 4> sv1;
    sv1.push_back(1);
    sv1.push_back(2);

    small_vector<int, 4> sv2{ std::move(sv1) };
    EXPECT_EQ(sv2.size(), 2);
    EXPECT_EQ(sv2.at_unchecked(0), 1);
    EXPECT_EQ(sv2.at_unchecked(1), 2);
    EXPECT_EQ(sv1.size(), 0);
}

TEST(small_vector, move_constructor_heap) {
    small_vector<int, 2> sv1;
    sv1.push_back(1);
    sv1.push_back(2);
    sv1.push_back(3); // triggers heap

    small_vector<int, 2> sv2{ std::move(sv1) };
    EXPECT_EQ(sv2.size(), 3);
    EXPECT_FALSE(sv2.is_static());
    EXPECT_EQ(sv2.at_unchecked(2), 3);
    EXPECT_EQ(sv1.size(), 0);
    EXPECT_TRUE(sv1.is_static());
}

TEST(small_vector, move_assignment) {
    small_vector<int, 2> sv1;
    sv1.push_back(1);
    sv1.push_back(2);
    sv1.push_back(3);

    small_vector<int, 2> sv2;
    sv2.push_back(99);

    sv2 = std::move(sv1);
    EXPECT_EQ(sv2.size(), 3);
    EXPECT_EQ(sv2.at_unchecked(0), 1);
    EXPECT_EQ(sv1.size(), 0);
}

} // namespace sl::meta
