//
// Created by usatiynyan.
//

#pragma once

#include <tl/optional.hpp>


namespace sl::meta {

template <typename Key, typename Value>
struct match {
    constexpr match(Key key) : key_{ std::move(key) }, value_{} {}
    constexpr match& case_(const Key& key, Value&& value) {
        if (!value_.has_value() && key_ == key) {
            value_ = std::move(value);
        }
        return *this;
    }
    constexpr tl::optional<Value> result() && { return std::move(value_); }

private:
    Key key_;
    tl::optional<Value> value_{};
};

} // namespace sl::meta
