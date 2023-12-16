//
// Created by usatiynyan on 12/14/23.
//

#include "sl/meta/lifetime.hpp"

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
    FinalizerObserver::Counters counters{0};
    ASSERT_EQ(counters[0], 0);
    {
        FinalizerObserver observer{ counters, 0 };
        ASSERT_EQ(counters[0], 0);
    }
    ASSERT_EQ(counters[0], 1);
}

TEST(lifetime, finalizerMove) {
    FinalizerObserver::Counters counters{0, 0};
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
        FinalizerObserver observer0{counters, 0};
        return observer0;
    };

    FinalizerObserver::Counters counters{0};
    {
        FinalizerObserver observer0 = makeObserver(counters);
        ASSERT_EQ(counters[0], 0);
    }
    ASSERT_EQ(counters[0], 1);
}

} // namespace sl::meta
