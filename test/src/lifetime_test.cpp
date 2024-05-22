//
// Created by usatiynyan on 12/14/23.
//

#include "sl/meta/lifetime.hpp"

#include "fixture/lifecycle.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

class FinalizerObserver : public finalizer<FinalizerObserver> {
public:
    using Counters = std::vector<std::size_t>;

public:
    FinalizerObserver(Counters& counters, std::size_t slot)
        : finalizer{ [](FinalizerObserver& self) { ++self.counter; } }, counter{ counters.at(slot) } {}

private:
    std::reference_wrapper<std::size_t> counter;
};

TEST(lifetime, finalizerDtor) {
    FinalizerObserver::Counters counters{ 0 };
    ASSERT_EQ(counters[0], 0);
    {
        FinalizerObserver observer{ counters, 0 };
        ASSERT_EQ(counters[0], 0);
    }
    ASSERT_EQ(counters[0], 1);
}

TEST(lifetime, finalizerMove) {
    FinalizerObserver::Counters counters{ 0, 0 };
    {
        FinalizerObserver observer0{ counters, 0 };
        FinalizerObserver observer1{ counters, 1 };
        ASSERT_EQ(counters, (FinalizerObserver::Counters{ 0, 0 }));
        observer0 = std::move(observer1);
        ASSERT_EQ(counters, (FinalizerObserver::Counters{ 1, 0 }));
    }
    ASSERT_EQ(counters, (FinalizerObserver::Counters{ 1, 1 }));
}

TEST(lifetime, finalizerRetval) {
    constexpr auto makeObserver = [](FinalizerObserver::Counters& counters) {
        FinalizerObserver observer0{ counters, 0 };
        return observer0;
    };

    FinalizerObserver::Counters counters{ 0 };
    {
        FinalizerObserver observer0 = makeObserver(counters);
        ASSERT_EQ(counters[0], 0);
    }
    ASSERT_EQ(counters[0], 1);
}

TEST(lifetime, derefPtr) {
    const auto guard = fixture::lifecycle::make_states_guard();

    {
        auto ptr = new fixture::lifecycle{ "ptr" };
        defer ptr_delete{ [ptr] { delete ptr; } };

        { auto copied_value = deref<decltype(ptr)&>(ptr); }
        const std::vector expected_copied_states{ fixture::lifecycle::state::copy_constructed,
                                                  fixture::lifecycle::state::destructed };
        EXPECT_EQ(fixture::lifecycle::states["ptr_copied"], expected_copied_states);

        { auto moved_value = deref<decltype(ptr)&&>(std::move(ptr)); }
        const std::vector expected_moved_states{ fixture::lifecycle::state::move_constructed,
                                                 fixture::lifecycle::state::destructed };
        EXPECT_EQ(fixture::lifecycle::states["ptr_moved"], expected_moved_states);
    }

    const std::vector expected_states{ fixture::lifecycle::state::constructed,
                                       fixture::lifecycle::state::copy_constructed_from,
                                       fixture::lifecycle::state::move_constructed_from,
                                       fixture::lifecycle::state::destructed };
    EXPECT_EQ(fixture::lifecycle::states["ptr"], expected_states);
};

TEST(lifetime, forwardDerefUniquePtr) {
    const auto guard = fixture::lifecycle::make_states_guard();

    {
        auto ptr = std::make_unique<fixture::lifecycle>("ptr");

        { auto copied_value = deref<decltype(ptr)&>(ptr); }
        const std::vector expected_copied_states{ fixture::lifecycle::state::copy_constructed,
                                                  fixture::lifecycle::state::destructed };
        EXPECT_EQ(fixture::lifecycle::states["ptr_copied"], expected_copied_states);

        { auto moved_value = deref<decltype(ptr)&&>(std::move(ptr)); }
        const std::vector expected_moved_states{ fixture::lifecycle::state::move_constructed,
                                                 fixture::lifecycle::state::destructed };
        EXPECT_EQ(fixture::lifecycle::states["ptr_moved"], expected_moved_states);
    }

    const std::vector expected_states{ fixture::lifecycle::state::constructed,
                                       fixture::lifecycle::state::copy_constructed_from,
                                       fixture::lifecycle::state::move_constructed_from,
                                       fixture::lifecycle::state::destructed };
    EXPECT_EQ(fixture::lifecycle::states["ptr"], expected_states);
};

TEST(lifetime, forwardDerefSharedPtr) {
    const auto guard = fixture::lifecycle::make_states_guard();

    {
        auto ptr = std::make_shared<fixture::lifecycle>("ptr");

        { auto copied_value = deref<decltype(ptr)&>(ptr); }
        const std::vector expected_copied_states{ fixture::lifecycle::state::copy_constructed,
                                                  fixture::lifecycle::state::destructed };
        EXPECT_EQ(fixture::lifecycle::states["ptr_copied"], expected_copied_states);

        { auto moved_value = deref<decltype(ptr)&&>(std::move(ptr)); }
        const std::vector expected_moved_states{ fixture::lifecycle::state::move_constructed,
                                                 fixture::lifecycle::state::destructed };
        EXPECT_EQ(fixture::lifecycle::states["ptr_moved"], expected_moved_states);
    }

    const std::vector expected_states{ fixture::lifecycle::state::constructed,
                                       fixture::lifecycle::state::copy_constructed_from,
                                       fixture::lifecycle::state::move_constructed_from,
                                       fixture::lifecycle::state::destructed };
    EXPECT_EQ(fixture::lifecycle::states["ptr"], expected_states);
};

TEST(lifetime, lazyEval) {
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

TEST(lifetime, lazyEvalMap) {
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
