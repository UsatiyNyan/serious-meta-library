//
// Created by usatiynyan.
//

#include "sl/meta/storage.hpp"

#include "fixture/lifecycle.hpp"
#include <gtest/gtest.h>

#include <functional>
#include <range/v3/algorithm/fold.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip_with.hpp>

namespace sl::meta {

TEST(storage, persistent) {
    const auto guard = fixture::lifecycle::make_states_guard();

    {
        persistent_storage<std::string, fixture::lifecycle> lifecycle_storage{ 128 };
        const std::string key{ "oraora" };

        {
            const auto emplace_result = lifecycle_storage.emplace(key, [] { return fixture::lifecycle{ "inserted" }; });
            ASSERT_TRUE(emplace_result.has_value());

            const auto& reference = emplace_result.value();
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
            const auto emplace_result =
                lifecycle_storage.emplace(key, [] { return fixture::lifecycle{ "not_inserted" }; });
            ASSERT_FALSE(emplace_result.has_value());

            const auto& reference = emplace_result.error();
            EXPECT_EQ(reference->id, "inserted");
            const std::vector<fixture::lifecycle::state> not_inserted_states;
            EXPECT_EQ(fixture::lifecycle::states["not_inserted"], not_inserted_states);
        }


        {
            const std::string other_key{ "mudamuda" };
            const auto emplace_result =
                lifecycle_storage.emplace(other_key, [] { return fixture::lifecycle{ "also_inserted" }; });
            ASSERT_TRUE(emplace_result.has_value());

            const auto& reference = emplace_result.value();
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

constexpr std::array<std::string_view, 2> inserted_persistent_array{
    "inserted1",
    "inserted2",
};

constexpr std::array<std::string_view, 3> also_inserted_persistent_array{
    "also_inserted1",
    "also_inserted2",
    "also_inserted3",
};

constexpr auto persistent_array_producer = ranges::views::transform([](std::string_view name) {
    return lazy_eval{ [name] { return fixture::lifecycle{ std::string{ name } }; } };
});

constexpr auto ranges_eq = [](const auto& xs, const auto& ys) {
    return ranges::fold_left(
        ranges::views::zip_with([](const auto& x, const auto& y) { return x == y; }, xs, ys),
        true,
        std::logical_and<bool>{}
    );
};

constexpr auto persistent_array_format = [](const auto& xs) {
    return ranges::fold_left(xs.span(), std::string{}, [](const auto& acc, const auto& x) { return acc + x.id + ','; });
};

TEST(storage, persistentArray) {
    const auto guard = fixture::lifecycle::make_states_guard();

    {
        persistent_array_storage<std::string, fixture::lifecycle> lifecycle_storage{ 128 };
        const std::string key{ "oraora" };

        {
            const auto emplace_result =
                lifecycle_storage.emplace(key, persistent_array_producer(inserted_persistent_array));
            ASSERT_TRUE(emplace_result.has_value());

            const auto& reference = emplace_result.value();
            EXPECT_TRUE(ranges_eq(reference.span(), inserted_persistent_array)) << persistent_array_format(reference);
            const std::vector inserted_states{ fixture::lifecycle::state::constructed };
            EXPECT_EQ(fixture::lifecycle::states["inserted1"], inserted_states);
            EXPECT_EQ(fixture::lifecycle::states["inserted2"], inserted_states);
        }

        {
            const auto maybe_reference = lifecycle_storage.lookup(key);
            ASSERT_TRUE(maybe_reference.has_value());

            const auto& reference = maybe_reference.value();
            EXPECT_TRUE(ranges_eq(reference.span(), inserted_persistent_array));
            const std::vector inserted_states{ fixture::lifecycle::state::constructed };
            EXPECT_EQ(fixture::lifecycle::states["inserted1"], inserted_states);
            EXPECT_EQ(fixture::lifecycle::states["inserted2"], inserted_states);
        }

        {
            constexpr std::array<std::string_view, 1> not_inserted_persistent_array{
                "not_inserted1",
            };
            const auto emplace_result =
                lifecycle_storage.emplace(key, persistent_array_producer(not_inserted_persistent_array));
            ASSERT_FALSE(emplace_result.has_value());

            const auto& reference = emplace_result.error();
            EXPECT_TRUE(ranges_eq(reference.span(), inserted_persistent_array));
            const std::vector<fixture::lifecycle::state> not_inserted_states;
            EXPECT_EQ(fixture::lifecycle::states["not_inserted1"], not_inserted_states);
        }

        {
            const std::string other_key{ "mudamuda" };
            const auto emplace_result =
                lifecycle_storage.emplace(other_key, persistent_array_producer(also_inserted_persistent_array));
            ASSERT_TRUE(emplace_result.has_value());

            const auto& reference = emplace_result.value();
            EXPECT_TRUE(ranges_eq(reference.span(), also_inserted_persistent_array));
            const std::vector inserted_states{ fixture::lifecycle::state::constructed };
            EXPECT_EQ(fixture::lifecycle::states["also_inserted1"], inserted_states);
            EXPECT_EQ(fixture::lifecycle::states["also_inserted2"], inserted_states);
            EXPECT_EQ(fixture::lifecycle::states["also_inserted3"], inserted_states);
        }
    }

    const std::vector after_inserted_states{ fixture::lifecycle::state::constructed,
                                             fixture::lifecycle::state::destructed };
    EXPECT_EQ(fixture::lifecycle::states["inserted1"], after_inserted_states);
    EXPECT_EQ(fixture::lifecycle::states["inserted2"], after_inserted_states);
    EXPECT_EQ(fixture::lifecycle::states["also_inserted1"], after_inserted_states);
    EXPECT_EQ(fixture::lifecycle::states["also_inserted2"], after_inserted_states);
    EXPECT_EQ(fixture::lifecycle::states["also_inserted3"], after_inserted_states);
}

TEST(storage, persistentArrayNesting) {
    const auto guard = fixture::lifecycle::make_states_guard();

    const std::string key{ "oraora" };
    const std::vector after_inserted_states{ fixture::lifecycle::state::constructed,
                                             fixture::lifecycle::state::destructed };

    {
        persistent_array_storage<std::string, fixture::lifecycle> outer{ 128 };
        (void)outer.emplace(key, persistent_array_producer(inserted_persistent_array));

        {
            persistent_array_storage<std::string, fixture::lifecycle> inner{ 128, outer };
            const std::string inner_key{ "mudamuda" };

            {
                const auto maybe_reference = inner.lookup(key);
                ASSERT_TRUE(maybe_reference.has_value());
                const auto& reference = maybe_reference.value();
                EXPECT_TRUE(ranges_eq(reference.span(), inserted_persistent_array));
            }

            {
                const auto maybe_reference = inner.lookup(key, 0);
                EXPECT_FALSE(maybe_reference.has_value());
            }

            (void)inner.emplace(key, persistent_array_producer(also_inserted_persistent_array));

            {
                const auto maybe_reference = inner.lookup(key);
                ASSERT_TRUE(maybe_reference.has_value());
                const auto& reference = maybe_reference.value();
                EXPECT_TRUE(ranges_eq(reference.span(), also_inserted_persistent_array));
            }

            {
                const auto maybe_reference = inner.lookup(key, 0);
                ASSERT_TRUE(maybe_reference.has_value());
                const auto& reference = maybe_reference.value();
                EXPECT_TRUE(ranges_eq(reference.span(), also_inserted_persistent_array));
            }
        }

        EXPECT_EQ(fixture::lifecycle::states["also_inserted1"], after_inserted_states);
        EXPECT_EQ(fixture::lifecycle::states["also_inserted2"], after_inserted_states);
        EXPECT_EQ(fixture::lifecycle::states["also_inserted3"], after_inserted_states);

        const std::vector inserted_states{ fixture::lifecycle::state::constructed };
        EXPECT_EQ(fixture::lifecycle::states["inserted1"], inserted_states);
        EXPECT_EQ(fixture::lifecycle::states["inserted2"], inserted_states);
    }

    EXPECT_EQ(fixture::lifecycle::states["inserted1"], after_inserted_states);
    EXPECT_EQ(fixture::lifecycle::states["inserted2"], after_inserted_states);
}

TEST(storage, uniqueString) {
    unique_string_storage storage{ 128 };
    ASSERT_TRUE(storage.memory().empty());

    {
        const auto maybe_entry = storage.lookup("oraora"_hsv);
        ASSERT_FALSE(maybe_entry.has_value());
    }

    {
        const auto emplace_result = storage.emplace("oraora"_hsv);
        ASSERT_TRUE(emplace_result.has_value());
        const auto& entry = emplace_result.value();
        ASSERT_EQ(entry.string_view(), "oraora");
    }

    ASSERT_EQ(storage.memory(), "oraora");

    {
        const auto maybe_entry = storage.lookup("oraora"_hsv);
        ASSERT_TRUE(maybe_entry.has_value());
        const auto& entry = maybe_entry.value();
        ASSERT_EQ(entry.string_view(), "oraora");
    }

    {
        const auto maybe_entry = storage.lookup("mudamuda"_hsv);
        ASSERT_FALSE(maybe_entry.has_value());
    }

    {
        const auto emplace_result = storage.emplace("mudamuda"_hsv);
        ASSERT_TRUE(emplace_result.has_value());
        const auto& entry = emplace_result.value();
        ASSERT_EQ(entry.string_view(), "mudamuda");
    }

    ASSERT_EQ(storage.memory(), "oraoramudamuda");

    {
        const auto maybe_entry = storage.lookup("oraora"_hsv);
        ASSERT_TRUE(maybe_entry.has_value());
        const auto& entry = maybe_entry.value();
        ASSERT_EQ(entry.string_view(), "oraora");
    }

    {
        const auto maybe_entry = storage.lookup("mudamuda"_hsv);
        ASSERT_TRUE(maybe_entry.has_value());
        const auto& entry = maybe_entry.value();
        ASSERT_EQ(entry.string_view(), "mudamuda");
    }
}

TEST(storage, uniqueStringNesting) {
    unique_string_storage outer{ 128 };
    (void)outer.emplace("oraora"_hsv);

    {
        unique_string_storage inner{ 128, outer };
        (void)inner.emplace("mudamuda"_hsv);

        {
            const auto maybe_entry = inner.lookup("oraora"_hsv, 0);
            ASSERT_FALSE(maybe_entry.has_value());
        }

        {
            const auto maybe_entry = inner.lookup("oraora"_hsv);
            ASSERT_TRUE(maybe_entry.has_value());
            const auto& entry = maybe_entry.value();
            ASSERT_EQ(entry.string_view(), "oraora");
        }

        {
            const auto maybe_entry = inner.lookup("mudamuda"_hsv, 0);
            ASSERT_TRUE(maybe_entry.has_value());
            const auto& entry = maybe_entry.value();
            ASSERT_EQ(entry.string_view(), "mudamuda");
        }
    }
}


} // namespace sl::meta
