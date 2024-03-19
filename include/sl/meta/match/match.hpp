//
// Created by usatiynyan.
//

#pragma once

#include <tl/optional.hpp>


namespace sl::meta {

template <typename Key, typename Value>
struct match {
    constexpr match(Key key) : key_{ std::move(key) } {}
    constexpr match& case_(const Key& key, const Value& value) {
        if (!value_.has_value() && key_ == key) {
            value_ = value;
        }
        return *this;
    }
    constexpr tl::optional<Value> result() && { return std::move(value_); }

private:
    Key key_;
    tl::optional<Value> value_{};
};

} // namespace sl::meta
