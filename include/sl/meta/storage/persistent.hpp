//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/func/const.hpp"
#include "sl/meta/lifetime/lazy_eval.hpp"

#include <tl/optional.hpp>
#include <tsl/robin_map.h>

#include <vector>

namespace sl::meta {

namespace detail {

template <typename T, template <typename> typename Alloc, bool is_const>
class persistent {
    using memory_t = add_const_if_t<std::vector<T, Alloc<T>>, is_const>;
    using deref_t = add_const_if_t<T, is_const>;

public:
    persistent(memory_t& memory, std::size_t address) : memory_{ &memory }, address_{ address } {}

    const T* operator->() const { return &memory_->at(address_); }
    const T& operator*() const { return memory_->at(address_); }

    deref_t* operator->() { return &memory_->at(address_); }
    deref_t& operator*() { return memory_->at(address_); }

private:
    memory_t* memory_;
    std::size_t address_;
};

} // namespace detail

template <typename T, template <typename> typename Alloc = std::allocator>
using persistent = detail::persistent<T, Alloc, /*is_const=*/false>;

template <typename T, template <typename> typename Alloc = std::allocator>
using const_persistent = detail::persistent<T, Alloc, /*is_const=*/true>;

template <
    typename Id,
    typename T,
    template <typename> typename Hash = std::hash,
    template <typename> typename Equal = std::equal_to,
    template <typename> typename Alloc = std::allocator>
class persistent_storage {
public:
    explicit persistent_storage(std::size_t capacity, tl::optional<persistent_storage&> parent = tl::nullopt)
        : memory_{}, address_table_{ capacity }, parent_{ parent } {
        memory_.reserve(capacity);
    }

    [[nodiscard]] tl::optional<const_persistent<T, Alloc>>
        lookup(const Id& item_id, std::size_t parent_recursion_limit = std::numeric_limits<std::size_t>::max()) const {
        return lookup_impl(*this, item_id, parent_recursion_limit);
    }

    [[nodiscard]] tl::optional<persistent<T, Alloc>>
        lookup(const Id& item_id, std::size_t parent_recursion_limit = std::numeric_limits<std::size_t>::max()) {
        return lookup_impl(*this, item_id, parent_recursion_limit);
    }

    // Does not lookup in parents before inserting.
    // No exceptions pls, they are not handled.
    template <typename MakeT>
        requires std::is_same_v<std::invoke_result_t<MakeT>, T>
    [[nodiscard]] std::pair<persistent<T, Alloc>, bool> emplace(const Id& id, MakeT&& make_item) {
        const std::size_t new_address = memory_.size();
        const auto [address_it, is_address_emplaced] = address_table_.try_emplace(id, new_address);
        auto result = std::make_pair(persistent<T, Alloc>{ memory_, address_it.value() }, is_address_emplaced);
        if (is_address_emplaced) {
            memory_.emplace_back(lazy_eval{ std::forward<MakeT>(make_item) });
        }
        return result;
    }

    [[nodiscard]] const auto& memory() const { return memory_; }

private:
    template <typename Self, bool is_const = std::is_const_v<Self>>
    static tl::optional<detail::persistent<T, Alloc, is_const>>
        lookup_impl(Self& self, const Id& item_id, std::size_t parent_recursion_limit) {
        if (const auto address_it = self.address_table_.find(item_id); address_it != self.address_table_.end()) {
            return detail::persistent<T, Alloc, is_const>{ self.memory_, address_it.value() };
        }
        if (parent_recursion_limit == 0) {
            return tl::nullopt;
        }
        return self.parent_.and_then([&item_id, &parent_recursion_limit](Self& p) {
            return lookup_impl(p, item_id, parent_recursion_limit - 1);
        });
    }

private:
    std::vector<T, Alloc<T>> memory_;
    tsl::robin_map<Id, std::size_t, Hash<Id>, Equal<Id>, Alloc<std::pair<Id, std::size_t>>> address_table_;
    tl::optional<persistent_storage&> parent_;
};

} // namespace sl::meta
