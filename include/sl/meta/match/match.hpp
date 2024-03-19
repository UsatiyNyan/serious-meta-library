//
// Created by usatiynyan.
//

#pragma once

#include <tl/optional.hpp>


namespace sl::meta {

template <typename Key, typename Value>
struct match {
    constexpr explicit match(Key key, tl::optional<Value> value = tl::nullopt)
        : key_{ std::move(key) }, value_{ std::move(value) } {}

    constexpr match case_(Key key, Value value) {
        if (!value_.has_value() && key_ == key) {
            return match{ std::move(key), std::move(value) };
        }
        return *this;
    }

    constexpr tl::optional<Value> result() && { return std::move(value_); }

private:
    Key key_;
    tl::optional<Value> value_{};
};

} // namespace sl::meta
