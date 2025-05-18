//
// Created by usatiynyan.
//

#include "sl/meta/func.hpp"

#include "sl/meta/fixture.hpp"

#include <gtest/gtest.h>
#include <tl/expected.hpp>

namespace sl::meta {

TEST(func, pipelineEmpty) {
    constexpr pipeline p;
    EXPECT_EQ(p(123), 123);
}

TEST(func, pipeline) {
    constexpr auto p = pipeline{} //
                           .then(SL_META_LIFT(std::to_string))
                           .then([](auto x) { return x + "abc"; })
                           .then([](auto x) { return x + x; });
    EXPECT_EQ(p(123), "123abc123abc");
    EXPECT_EQ(p(10.1f), "10.100000abc10.100000abc");
}

TEST(func, lazyEval) {
    const auto guard = fixture::lifecycle::make_states_guard();

    lazy_eval make_value{ [] { return fixture::lifecycle{ "value" }; } };
    const std::vector<fixture::lifecycle::state> before_lazy_eval_states;
    EXPECT_EQ(fixture::lifecycle::states["value"], before_lazy_eval_states);

    {
        const fixture::lifecycle value{ std::move(make_value) };
        const std::vector lazy_eval_states{ fixture::lifecycle::state::constructed };
        EXPECT_EQ(fixture::lifecycle::states["value"], lazy_eval_states);
    }


    const std::vector after_lazy_eval_states{ fixture::lifecycle::state::constructed,
                                              fixture::lifecycle::state::destructed };
    EXPECT_EQ(fixture::lifecycle::states["value"], after_lazy_eval_states);
}

TEST(func, lazyEvalMap) {
    const auto guard = fixture::lifecycle::make_states_guard();

    {
        std::map<std::string, fixture::lifecycle> map;
        const std::string key{ "oraora" };

        (void)map.try_emplace(key, lazy_eval{ [] { return fixture::lifecycle{ "inserted" }; } });
        const std::vector lazy_eval_states{ fixture::lifecycle::state::constructed };
        EXPECT_EQ(fixture::lifecycle::states["inserted"], lazy_eval_states);

        auto [it, is_emplaced] = map.try_emplace(key, lazy_eval{ [] { return fixture::lifecycle{ "not_inserted" }; } });
        ASSERT_FALSE(is_emplaced);
        const std::vector<fixture::lifecycle::state> not_lazy_eval_states;
        EXPECT_EQ(fixture::lifecycle::states["not_inserted"], not_lazy_eval_states);
    }

    const std::vector after_lazy_eval_states{ fixture::lifecycle::state::constructed,
                                              fixture::lifecycle::state::destructed };
    EXPECT_EQ(fixture::lifecycle::states["inserted"], after_lazy_eval_states);
}

} // namespace sl::meta
