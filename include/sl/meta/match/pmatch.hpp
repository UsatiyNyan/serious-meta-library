//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/lifetime/deref.hpp"
#include "sl/meta/match/overloaded.hpp"
#include "sl/meta/traits/treat_as.hpp"

#include <variant>

#include <tl/optional.hpp>

namespace sl::meta {

template <typename... Fs>
class pmatch {
public:
    constexpr explicit pmatch(Fs&&... fs) : overloaded_{ std::forward<Fs>(fs)... } {}

    template <treat_as_ptr T>
    constexpr auto operator()(T&& v) const {
        if (v == nullptr) {
            return overloaded_();
        }
        return overloaded_(deref<T>(std::forward<T>(v)));
    }

    template <treat_as_optional T>
    constexpr auto operator()(T&& v) const {
        if (!v.has_value()) {
            return overloaded_();
        }
        return overloaded_(std::forward<T>(v).value());
    }

    template <treat_as_expected T>
    constexpr auto operator()(T&& v) const {
        if (!v.has_value()) {
            return overloaded_(std::forward<T>(v).error());
        }
        return overloaded_(std::forward<T>(v).value());
    }

    template <treat_as_variant T>
    constexpr auto operator()(T&& v) const {
        return std::visit(overloaded_, std::forward<T>(v));
    }

private:
    overloaded<Fs...> overloaded_;
};

template <typename T, typename... Fs>
constexpr auto operator|(T&& v, const pmatch<Fs...>& pm) {
    return pm(std::forward<T>(v));
}

} // namespace sl::meta
