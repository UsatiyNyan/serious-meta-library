//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/lifetime/deref.hpp"
#include "sl/meta/match/overloaded.hpp"
#include "sl/meta/traits/concept.hpp"

#include <variant>

namespace sl::meta {

template <typename... Fs>
class pmatch {
public:
    constexpr explicit pmatch(Fs&&... fs) : overloaded_{ std::forward<Fs>(fs)... } {}

    template <typename TV>
        requires PtrLike<std::decay_t<TV>>
    constexpr auto operator()(TV&& v) const {
        if (v == nullptr) {
            return overloaded_();
        }
        return overloaded_(deref(std::forward<TV>(v)));
    }

    template <typename TV>
        requires OptionalLike<std::decay_t<TV>>
    constexpr auto operator()(TV&& v) const {
        if (!v.has_value()) {
            return overloaded_();
        }
        return overloaded_(std::forward<TV>(v).value());
    }

    template <typename TV>
        requires ExpectedLike<std::decay_t<TV>>
    constexpr auto operator()(TV&& v) const {
        if (!v.has_value()) {
            return overloaded_(std::forward<TV>(v).error());
        }
        return overloaded_(std::forward<TV>(v).value());
    }

    template <typename TV>
        requires Variant<std::decay_t<TV>>
    constexpr auto operator()(TV&& v) const {
        return std::visit(overloaded_, std::forward<TV>(v));
    }

private:
    overloaded<Fs...> overloaded_;
};

template <typename TV, typename... Fs>
constexpr auto operator|(TV&& v, const pmatch<Fs...>& pm) {
    return pm(std::forward<TV>(v));
}

} // namespace sl::meta
