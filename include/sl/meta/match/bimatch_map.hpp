//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/match/bimatch.hpp"


namespace sl::meta {
namespace detail {

template <typename F, typename Key, typename Value>
concept BimatchProducer = //
    requires(F f, bimatch<Key, Value> m) {
        { f(m) };
        { f(m) } -> std::same_as<bimatch<Key, Value>>;
    };
} // namespace detail

template <typename Key, typename Value, detail::BimatchProducer<Key, Value> BimatchProducer>
struct bimatch_map {
    constexpr explicit bimatch_map(BimatchProducer producer) : producer_{ std::move(producer) } {}
    constexpr tl::optional<Value> value(const Key& key) const {
        return producer_(bimatch<Key, Value>{ key, bimatch_key_tag }).value();
    }
    constexpr tl::optional<Key> key(const Value& value) const {
        return producer_(bimatch<Key, Value>{ value, bimatch_value_tag }).key();
    }

private:
    BimatchProducer producer_;
};

template <typename Key, typename Value>
constexpr auto make_bimatch_map(auto producer) {
    return bimatch_map<Key, Value, decltype(producer)>{ std::move(producer) };
}

} // namespace sl::meta
