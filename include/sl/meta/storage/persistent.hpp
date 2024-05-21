//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/lifetime/lazy_eval.hpp"

#include <tl/optional.hpp>
#include <tsl/robin_map.h>

#include <type_traits>
#include <vector>

namespace sl::meta {
template <
    typename Id,
    typename T,
    template <typename> typename Hash = std::hash,
    template <typename> typename Equal = std::equal_to,
    template <typename> typename Alloc = std::allocator>
class persistent_storage {
public:
    class reference;

public:
    explicit persistent_storage(std::size_t capacity, tl::optional<const persistent_storage&> parent = tl::nullopt)
        : memory_{}, address_table_{ capacity }, parent_{ parent } {
        memory_.reserve(capacity);
    }

    [[nodiscard]] tl::optional<reference>
        lookup(const Id& item_id, std::size_t parent_recursion_limit = std::numeric_limits<std::size_t>::max()) const {
        if (const auto address_it = address_table_.find(item_id); address_it != address_table_.end()) {
            return reference{ memory_, address_it.value() };
        }
        if (parent_recursion_limit == 0) {
            return tl::nullopt;
        }
        return parent_.and_then([&item_id, &parent_recursion_limit](const persistent_storage& p) {
            return p.lookup(item_id, parent_recursion_limit - 1);
        });
    }

    // Does not lookup in parents before inserting.
    // No exceptions pls, they are not handled.
    template <typename MakeT>
        requires std::is_same_v<std::invoke_result_t<MakeT>, T>
    [[nodiscard]] std::pair<reference, bool> emplace(const Id& id, MakeT&& make_item) {
        const std::size_t new_address = memory_.size();
        const auto [address_it, is_address_emplaced] = address_table_.try_emplace(id, new_address);
        auto result = std::make_pair(reference{ memory_, address_it.value() }, is_address_emplaced);
        if (is_address_emplaced) {
            memory_.emplace_back(lazy_eval{ std::forward<MakeT>(make_item) });
        }
        return result;
    }

    [[nodiscard]] const auto& memory() const { return memory_; }

private:
    std::vector<T, Alloc<T>> memory_;
    tsl::robin_map<Id, std::size_t, Hash<Id>, Equal<Id>, Alloc<std::pair<Id, T>>> address_table_;
    tl::optional<const persistent_storage&> parent_;
};

template <
    typename Id,
    typename T,
    template <typename>
    typename Hash,
    template <typename>
    typename Equal,
    template <typename>
    typename Alloc>
class persistent_storage<Id, T, Hash, Equal, Alloc>::reference {
public:
    reference(const std::vector<T, Alloc<T>>& memory, std::size_t address) : memory_{ memory }, address_{ address } {}

    const T* operator->() const { return &memory_.at(address_); }
    const T& operator*() const { return memory_.at(address_); }

private:
    const std::vector<T, Alloc<T>>& memory_;
    std::size_t address_;
};

} // namespace sl::meta
