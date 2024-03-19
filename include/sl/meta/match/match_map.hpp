//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/match/match.hpp"


namespace sl::meta {
namespace detail {

template <typename F, typename Key, typename Value>
concept MatchProducer = //
    requires(F f, match<Key, Value> m) {
        { f(m) };
        { f(m) } -> std::same_as<match<Key, Value>>;
    };
} // namespace detail

template <typename Key, typename Value, detail::MatchProducer<Key, Value> MatchProducer>
struct match_map {
    constexpr explicit match_map(MatchProducer producer) : producer_{ std::move(producer) } {}
    constexpr tl::optional<Value> operator[](const Key& key) const {
        return producer_(match<Key, Value>{ key }).result();
    }

private:
    MatchProducer producer_;
};

template <typename Key, typename Value>
constexpr auto make_match_map(auto producer) {
    return match_map<Key, Value, decltype(producer)>{ std::move(producer) };
}

} // namespace sl::meta
