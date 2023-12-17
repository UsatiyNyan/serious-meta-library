//
// Created by usatiynyan on 12/17/23.
//

#include "sl/meta/field.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

TEST(field, alignedField) {
    struct AlignedFieldAggregate {
        aligned_field<int> i;
        aligned_field<char, int> c;
    };
    static_assert(is_tightly_packed<AlignedFieldAggregate>);
    static_assert(sizeof(AlignedFieldAggregate) == sizeof(int) * 2);
}

TEST(field, isTightlyPacked) {
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

} // namespace sl::meta
