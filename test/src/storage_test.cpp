//
// Created by usatiynyan.
//

#include "sl/meta/func/lazy_eval.hpp"
#include "sl/meta/storage.hpp"

#include "sl/meta/fixture.hpp"

#include <gtest/gtest.h>

#include <functional>
#include <range/v3/algorithm/fold.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip_with.hpp>

namespace sl::meta {

TEST(storage, persistent) {
    const auto guard = fixture::lifecycle::make_states_guard();

    {
        // guarantee that items are not copied
        persistent_storage<std::string, fixture::lifecycle> lifecycle_storage{ { .memory_capacity = 128 } };
        const std::string key{ "oraora" };

        {
            const auto [value, is_emplaced] = lifecycle_storage.emplace(key, "inserted");
            ASSERT_TRUE(is_emplaced);
            EXPECT_EQ(value->id, "inserted");
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
            const auto [value, is_emplaced] = lifecycle_storage.emplace(key, "not_inserted");
            ASSERT_FALSE(is_emplaced);
            EXPECT_EQ(value->id, "inserted");
            const std::vector<fixture::lifecycle::state> not_inserted_states;
            EXPECT_EQ(fixture::lifecycle::states["not_inserted"], not_inserted_states);
        }

        {
            const std::string other_key{ "mudamuda" };
            const auto [value, is_emplaced] = lifecycle_storage.emplace(other_key, "also_inserted");
            ASSERT_TRUE(is_emplaced);
            EXPECT_EQ(value->id, "also_inserted");
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
        persistent_storage<std::string, fixture::lifecycle> outer;
        std::ignore = outer.emplace(key, "outer");

        {
            persistent_storage<std::string, fixture::lifecycle> inner{ { .get_parent = [&] { return &outer; } } };
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

            std::ignore = inner.emplace(key, "inner");

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
        persistent_array_storage<std::string, fixture::lifecycle> lifecycle_storage{ { .memory_capacity = 128 } };
        const std::string key{ "oraora" };

        {
            const auto [value, is_emplaced] =
                lifecycle_storage.emplace(key, persistent_array_producer(inserted_persistent_array));
            ASSERT_TRUE(is_emplaced);
            EXPECT_TRUE(ranges_eq(value.span(), inserted_persistent_array)) << persistent_array_format(value);
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
            const auto [value, is_emplaced] =
                lifecycle_storage.emplace(key, persistent_array_producer(not_inserted_persistent_array));
            ASSERT_FALSE(is_emplaced);
            EXPECT_TRUE(ranges_eq(value.span(), inserted_persistent_array));
            const std::vector<fixture::lifecycle::state> not_inserted_states;
            EXPECT_EQ(fixture::lifecycle::states["not_inserted1"], not_inserted_states);
        }

        {
            const std::string other_key{ "mudamuda" };
            const auto [value, is_emplaced] =
                lifecycle_storage.emplace(other_key, persistent_array_producer(also_inserted_persistent_array));
            ASSERT_TRUE(is_emplaced);
            EXPECT_TRUE(ranges_eq(value.span(), also_inserted_persistent_array));
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
        persistent_array_storage<std::string, fixture::lifecycle> outer{ { .memory_capacity = 128 } };
        std::ignore = outer.emplace(key, persistent_array_producer(inserted_persistent_array));

        {
            persistent_array_storage<std::string, fixture::lifecycle> inner{
                { .get_parent = [&] { return &outer; }, .memory_capacity = 128 },
            };
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

            std::ignore = inner.emplace(key, persistent_array_producer(also_inserted_persistent_array));

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
    unique_string_storage storage;
    ASSERT_TRUE(storage.memory_str().empty());

    {
        const auto maybe_entry = storage.lookup("oraora"_hsv);
        ASSERT_FALSE(maybe_entry.has_value());
    }

    {
        const auto [entry, is_emplaced] = storage.emplace("oraora"_hsv);
        ASSERT_TRUE(is_emplaced);
        ASSERT_EQ(entry.string_view(), "oraora");
    }

    ASSERT_EQ(storage.memory_str(), "oraora");

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
        const auto [entry, is_emplaced] = storage.emplace("mudamuda"_hsv);
        ASSERT_TRUE(is_emplaced);
        ASSERT_EQ(entry.string_view(), "mudamuda");
    }

    ASSERT_EQ(storage.memory_str(), "oraoramudamuda");

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
    unique_string_storage outer;
    std::ignore = outer.emplace("oraora"_hsv);

    {
        unique_string_storage inner{ { .get_parent = [&] { return &outer; } } };
        std::ignore = inner.emplace("mudamuda"_hsv);

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

TEST(storage, uniqueStringLiteral) {
    unique_string_storage storage;
    const unique_string entry = "oraora"_us(storage);
    ASSERT_EQ(entry.string_view(), "oraora");
    ASSERT_EQ(storage.memory_str(), "oraora");
    const auto maybe_entry = storage.lookup("oraora"_hsv);
    ASSERT_TRUE(maybe_entry.has_value());
}

TEST(storage, uniqueFormatStringLiteral) {
    unique_string_storage storage;
    const unique_string entry = "oraora{}"_ufs("mudamuda")(storage);
    ASSERT_EQ(entry.string_view(), "oraoramudamuda");
    ASSERT_EQ(storage.memory_str(), "oraoramudamuda");
    const auto maybe_entry = storage.lookup("oraoramudamuda"_hsv);
    ASSERT_TRUE(maybe_entry.has_value());
}

TEST(storage, uniqueFormatString) {
    unique_string_storage storage;
    const unique_string entry = "oraora"_us(storage);
    ASSERT_EQ(fmt::format("{}", entry), "oraora");
}

} // namespace sl::meta
