//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/lifetime/defer.hpp"
#include "sl/meta/lifetime/lazy_eval.hpp"

#include <function2/function2.hpp>
#include <tl/optional.hpp>
#include <tsl/robin_map.h>

namespace sl::meta {

template <typename K, typename V>
class storage {
    struct instance {
        V value;
        std::size_t counter;
    };

public:
    struct erase {
        storage& self;
        K key;
        void operator()() const { self.erase_impl(key); }
    };
    using defer_erase = meta::defer<fu2::capacity_can_hold<erase>>;
    struct borrow_result {
        V& reference;
        defer_erase defer;
    };

public:
    template <typename MakeValue>
    [[nodiscard]] borrow_result emplace(const K& key, MakeValue&& make_value) {
        const auto [it, is_emplaced] = instances_.try_emplace(
            key, //
            lazy_eval{ [make_value = std::forward<MakeValue>(make_value)] {
                return instance{
                    .value = make_value(),
                    .counter = 0,
                };
            } }
        );
        ++it.value().counter;
        return borrow_result{
            .reference = it.value().value,
            .defer{ erase{ *this, key } },
        };
    }
    [[nodiscard]] tl::optional<borrow_result> borrow(const K& key) {
        return find_impl(instances_, key).map([&](instance& x) {
            ++x.counter;
            return borrow_result{
                .reference = x.value,
                .defer{ erase{ *this, key } },
            };
        });
    }

    [[nodiscard]] tl::optional<const V&> get(const K& key) const {
        return find_impl(instances_, key).map([](const instance& x) { return x.value; });
    }
    [[nodiscard]] tl::optional<V&> get(const K& key) {
        return find_impl(instances_, key).map([](instance& x) { return x.value; });
    }

private:
    void erase_impl(const K& key) {
        const auto it = instances_.find(key);
        if (it == instances_.end()) {
            return;
        }
        std::size_t& counter = it.value().counter;
        if (counter != 0) [[likely]] {
            --counter;
        }
        if (counter != 0) {
            return;
        }
        instances_.erase(it);
    }

    template <typename Instances>
    static auto find_impl(Instances instances, const K& key) {
        using return_type = tl::optional<std::conditional_t< //
            /*_Cond=*/std::is_const_v<Instances>,
            /*_Iftrue=*/const instance&,
            /*_Iffalse=*/instance&>>;
        const auto it = instances.find(key);
        if (it == instances.end() || it->second.counter == 0) {
            return return_type{ tl::nullopt };
        }
        return return_type{ it.value() };
    }

private:
    tsl::robin_map<K, instance> instances_;
};

} // namespace sl::meta
