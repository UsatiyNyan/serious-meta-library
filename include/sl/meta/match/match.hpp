//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/monad/maybe.hpp"

namespace sl::meta {

template <typename Key, typename Value>
struct match {
    constexpr explicit match(Key key, maybe<Value> value = null)
        : key_{ std::move(key) }, value_{ std::move(value) } {}

    constexpr match case_(Key key, Value value) {
        if (!value_.has_value() && key_ == key) {
            return match{ std::move(key), std::move(value) };
        }
        return *this;
    }

    constexpr maybe<Value> result() && { return std::move(value_); }

private:
    Key key_;
    maybe<Value> value_{};
};

} // namespace sl::meta
