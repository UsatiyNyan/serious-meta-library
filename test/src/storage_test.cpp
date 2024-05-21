//
// Created by usatiynyan.
//

#include "sl/meta/storage.hpp"

#include "fixture/lifecycle.hpp"
#include <gtest/gtest.h>

namespace sl::meta {

TEST(storage, persistent) {
    const auto guard = fixture::lifecycle::make_states_guard();

    {
        persistent_storage<std::string, fixture::lifecycle> lifecycle_storage{ 128 };
        const std::string key{ "oraora" };

        {
            const auto [reference, is_emplaced] =
                lifecycle_storage.emplace(key, [] { return fixture::lifecycle{ "inserted" }; });

            ASSERT_TRUE(is_emplaced);
            EXPECT_EQ(reference->id, "inserted");
            const std::vector lazy_eval_states{ fixture::lifecycle::state::constructed };
            EXPECT_EQ(fixture::lifecycle::states["inserted"], lazy_eval_states);
        }

        {
            const auto maybe_reference = lifecycle_storage.lookup(key);

            ASSERT_TRUE(maybe_reference.has_value());
            const auto& reference = maybe_reference.value();
            EXPECT_EQ(reference->id, "inserted");
            const std::vector lazy_eval_states{ fixture::lifecycle::state::constructed };
            EXPECT_EQ(fixture::lifecycle::states["inserted"], lazy_eval_states);
        }

        {
            const auto [reference, is_emplaced] =
                lifecycle_storage.emplace(key, [] { return fixture::lifecycle{ "not_inserted" }; });

            ASSERT_FALSE(is_emplaced);
            EXPECT_EQ(reference->id, "inserted");
            const std::vector<fixture::lifecycle::state> not_lazy_eval_states;
            EXPECT_EQ(fixture::lifecycle::states["not_inserted"], not_lazy_eval_states);
        }
    }

    const std::vector after_lazy_eval_states{ fixture::lifecycle::state::constructed,
                                              fixture::lifecycle::state::destructed };
    EXPECT_EQ(fixture::lifecycle::states["inserted"], after_lazy_eval_states);
}

TEST(storage, persistentNesting) {}

} // namespace sl::meta
