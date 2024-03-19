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
    constexpr explicit bimatch(Key key, bimatch_key_flag_t = {}) : key_{ std::move(key) } {}
    constexpr explicit bimatch(Value value, bimatch_value_flag_t = {}) : value_{ std::move(value) } {}
    constexpr bimatch(Key key, Value value) : key_{ std::move(key) }, value_{ std::move(value) } {}

    constexpr bimatch case_(Key key, Value value) {
        if (!value_.has_value() && key_.has_value() && key_ == key) {
            return bimatch{ std::move(key), std::move(value) };
        }
        if (!key_.has_value() && value_.has_value() && value_ == value) {
            return bimatch{ std::move(key), std::move(value) };
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
