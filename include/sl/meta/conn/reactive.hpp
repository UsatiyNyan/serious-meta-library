//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/monad/maybe.hpp"

#include <cstddef>
#include <memory>

namespace sl::meta::reactive {
namespace detail {

template <typename T>
struct state {
    meta::maybe<T> value{};
    std::uint32_t version = 0;

    constexpr meta::maybe<const T&> peek() const& {
        return value.map([](const T& v) -> const T& { return v; });
    }
};

} // namespace detail

template <typename T>
struct [[nodiscard]] observer {
    constexpr observer(detail::state<T>* state) : state_{ state } {}

    constexpr meta::maybe<const T&> peek() const& { return state_->peek(); }

    [[nodiscard]] constexpr meta::maybe<const T&> consume() & {
        if (!has_update()) {
            return meta::null;
        }
        observed_version_.emplace(state_->version);
        return state_->value.value();
    }

    constexpr void touch() & { observed_version_.reset(); }

    constexpr bool has_update() const& { return state_->value.has_value() && observed_version_ != state_->version; }

private:
    meta::maybe<std::uint32_t> observed_version_{};
    detail::state<T>* state_;
};

template <typename T>
struct state {
    constexpr state() = default;

    template <typename... Args>
    constexpr explicit state(Args&&... args) {
        std::ignore = set(std::forward<Args>(args)...);
    }

    constexpr observer<T> make_observer() & { return observer{ state_.get() }; }

public:
    template <typename... Args>
    constexpr std::uint32_t set(Args&&... args) & {
        state_->value.emplace(std::forward<Args>(args)...);
        return state_->version++;
    }

    constexpr std::uint32_t set_if_ne(T value) & {
        if (value == state_->value) {
            return state_->version;
        }
        return set(std::move(value));
    }

    constexpr meta::maybe<const T&> peek() const& { return state_->peek(); }

private:
    std::unique_ptr<detail::state<T>> state_ = std::make_unique<detail::state<T>>();
};

template <typename... Ts>
meta::maybe<std::tuple<const Ts&...>> all(observer<Ts>&... os) {
    if (!(os.has_update() && ... && true)) {
        return meta::null;
    }
    return std::tuple<const Ts&...>{ os.consume().value()... };
}

} // namespace sl::meta::reactive
