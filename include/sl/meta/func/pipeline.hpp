//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/func/identity.hpp"

namespace sl::meta {

template <typename F = identity_t>
class pipeline {
public:
    constexpr explicit pipeline(F f = F{}) : f_{ std::move(f) } {}

    template <typename G>
    constexpr auto then(G&& g) && {
        auto combined = [g = std::forward<G>(g), f = std::move(*this)](auto&& arg) {
            return g(f(std::forward<decltype(arg)>(arg)));
        };
        return pipeline<decltype(combined)>{std::move(combined)};
    }

    template <typename Arg>
    constexpr auto operator()(Arg arg) const {
        return f_(std::forward<Arg>(arg));
    }

private:
    F f_;
};

template <typename F>
pipeline(F f) -> pipeline<F>;

} // namespace sl::meta
