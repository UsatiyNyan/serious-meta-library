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
            const std::vector inserted_states{ fixture::lifecycle::state::constructed };
            EXPECT_EQ(fixture::lifecycle::states["inserted"], inserted_states);
        }

        {
            const auto maybe_reference = lifecycle_storage.lookup(key);

            ASSERT_TRUE(maybe_reference.has_value());
            const auto& reference = maybe_reference.value();
            EXPECT_EQ(reference->id, "inserted");
            const std::vector inserted_states{ fixture::lifecycle::state::constructed };
            EXPECT_EQ(fixture::lifecycle::states["inserted"], inserted_states);
        }

        {
            const auto [reference, is_emplaced] =
                lifecycle_storage.emplace(key, [] { return fixture::lifecycle{ "not_inserted" }; });

            ASSERT_FALSE(is_emplaced);
            EXPECT_EQ(reference->id, "inserted");
            const std::vector<fixture::lifecycle::state> not_inserted_states;
            EXPECT_EQ(fixture::lifecycle::states["not_inserted"], not_inserted_states);
        }


        {
            const std::string other_key{ "mudamuda" };
            const auto [reference, is_emplaced] =
                lifecycle_storage.emplace(other_key, [] { return fixture::lifecycle{ "also_inserted" }; });

            ASSERT_TRUE(is_emplaced);
            EXPECT_EQ(reference->id, "also_inserted");
            const std::vector inserted_states{ fixture::lifecycle::state::constructed };
            EXPECT_EQ(fixture::lifecycle::states["also_inserted"], inserted_states);
        }
    }

    const std::vector after_inserted_states{ fixture::lifecycle::state::constructed,
                                             fixture::lifecycle::state::destructed };
    EXPECT_EQ(fixture::lifecycle::states["inserted"], after_inserted_states);
    EXPECT_EQ(fixture::lifecycle::states["also_inserted"], after_inserted_states);
}

TEST(storage, persistentNesting) {
    const auto guard = fixture::lifecycle::make_states_guard();

    const std::string key{ "oraora" };
    const std::vector after_inserted_states{ fixture::lifecycle::state::constructed,
                                             fixture::lifecycle::state::destructed };

    {
        persistent_storage<std::string, fixture::lifecycle> outer{ 128 };
        (void)outer.emplace(key, [] { return fixture::lifecycle{ "outer" }; });

        {
            persistent_storage<std::string, fixture::lifecycle> inner{ 128, outer };
            const std::string inner_key{ "mudamuda" };

            {
                const auto maybe_reference = inner.lookup(key);
                ASSERT_TRUE(maybe_reference.has_value());
                const auto& reference = maybe_reference.value();
                EXPECT_EQ(reference->id, "outer");
            }

            {
                const auto maybe_reference = inner.lookup(key, 0);
                EXPECT_FALSE(maybe_reference.has_value());
            }

            (void)inner.emplace(key, [] { return fixture::lifecycle{ "inner" }; });

            {
                const auto maybe_reference = inner.lookup(key);
                ASSERT_TRUE(maybe_reference.has_value());
                const auto& reference = maybe_reference.value();
                EXPECT_EQ(reference->id, "inner");
            }

            {
                const auto maybe_reference = inner.lookup(key, 0);
                ASSERT_TRUE(maybe_reference.has_value());
                const auto& reference = maybe_reference.value();
                EXPECT_EQ(reference->id, "inner");
            }
        }

        EXPECT_EQ(fixture::lifecycle::states["inner"], after_inserted_states);

        const std::vector inserted_states{ fixture::lifecycle::state::constructed };
        EXPECT_EQ(fixture::lifecycle::states["outer"], inserted_states);
    }

    EXPECT_EQ(fixture::lifecycle::states["outer"], after_inserted_states);
}

} // namespace sl::meta
