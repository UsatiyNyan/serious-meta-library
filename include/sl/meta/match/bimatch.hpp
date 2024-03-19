//
// Created by usatiynyan.
//

#pragma once

#include <tl/optional.hpp>


namespace sl::meta {

struct bimatch_key_flag_t {};
constexpr bimatch_key_flag_t bimatch_key_tag{};
struct bimatch_value_flag_t {};
constexpr bimatch_value_flag_t bimatch_value_tag{};

template <typename Key, typename Value>
struct bimatch {
    constexpr bimatch(Key key, bimatch_key_flag_t = {}) : key_{ std::move(key) } {}
    constexpr bimatch(Value value, bimatch_value_flag_t = {}) : value_{ std::move(value) } {}
    constexpr bimatch& case_(const Key& key, const Value& value) {
        if (!value_.has_value() && key_.has_value() && key_ == key) {
            value_ = value;
        }
        if (!key_.has_value() && value_.has_value() && value_ == value) {
            key_ = key;
        }
        return *this;
    }
    constexpr tl::optional<Value> value() && { return std::move(value_); }
    constexpr tl::optional<Key> key() && { return std::move(key_); }

private:
    tl::optional<Key> key_{};
    tl::optional<Value> value_{};
};

} // namespace sl::meta
