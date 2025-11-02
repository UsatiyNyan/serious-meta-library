//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/func/function.hpp"
#include "sl/meta/monad/maybe.hpp"
#include "sl/meta/traits/unique.hpp"
#include "sl/meta/type/const.hpp"

#include <tsl/robin_map.h>

#include <type_traits>
#include <vector>

namespace sl::meta {
namespace detail {

template <typename T, template <typename> typename Alloc, bool IsConst>
class persistent {
    using memory_type = type::add_const_if_t<std::vector<T, Alloc<T>>, IsConst>;
    using deref_type = type::add_const_if_t<T, IsConst>;

public:
    persistent(memory_type& memory, std::size_t address) : memory_{ &memory }, address_{ address } {}

    const T* operator->() const { return ptr(); }
    [[nodiscard]] const T& operator*() const { return *ptr(); }

    deref_type* operator->() { return ptr(); }
    [[nodiscard]] deref_type& operator*() { return *ptr(); }

private:
    deref_type* ptr() const { return memory_->data() + address_; }

private:
    memory_type* memory_;
    std::size_t address_;
};

} // namespace detail

template <typename T, template <typename> typename Alloc = std::allocator>
using persistent = detail::persistent<T, Alloc, /*IsConst=*/false>;

template <typename T, template <typename> typename Alloc = std::allocator>
using const_persistent = detail::persistent<T, Alloc, /*IsConst=*/true>;

template <
    typename Id,
    typename T,
    template <typename> typename Hash = std::hash,
    template <typename> typename Equal = std::equal_to,
    template <typename> typename Alloc = std::allocator>
struct persistent_storage : immovable {
    using memory_alloc_type = Alloc<T>;
    using memory_type = std::vector<T, memory_alloc_type>;

    using value_type = persistent<T, Alloc>;
    using const_value_type = const_persistent<T, Alloc>;
    using table_alloc_type = Alloc<std::pair<Id, std::size_t>>;
    using table_type = tsl::robin_map<Id, std::size_t, Hash<Id>, Equal<Id>, table_alloc_type>;

    using get_parent_type = unique_function<persistent_storage*() const>;

    struct init_type {
        get_parent_type get_parent = {};
        std::size_t memory_capacity = 0;
        std::size_t table_capacity = 0;
        const memory_alloc_type& memory_alloc = {};
        const table_alloc_type& table_alloc = {};
    };

public:
    explicit persistent_storage(init_type init = {})
        : memory_{ init.memory_alloc }, table_{ init.table_capacity }, get_parent_{ std::move(init.get_parent) } {
        memory_.reserve(init.memory_capacity);
    }

    [[nodiscard]] maybe<const_value_type>
        lookup(const Id& item_id, std::size_t parent_recursion_limit = std::numeric_limits<std::size_t>::max()) const {
        return lookup_impl(*this, item_id, parent_recursion_limit);
    }

    [[nodiscard]] maybe<value_type>
        lookup(const Id& item_id, std::size_t parent_recursion_limit = std::numeric_limits<std::size_t>::max()) {
        return lookup_impl(*this, item_id, parent_recursion_limit);
    }

    // Does not lookup in parents before inserting.
    template <typename... Args>
    [[nodiscard]] std::pair<value_type, bool> emplace(const Id& id, Args&&... args) {
        auto [value, is_emplaced] = new_addr_impl(id);
        if (is_emplaced) {
            memory_.emplace_back(std::forward<Args>(args)...);
        }
        return std::make_pair(std::move(value), is_emplaced);
    }

    // Does not lookup in parents before inserting.
    template <typename... Args>
    [[nodiscard]] std::pair<value_type, bool> insert(const Id& id, Args&&... args) {
        auto [value, is_emplaced] = new_addr_impl(id);
        if (is_emplaced) {
            memory_.emplace_back(std::forward<Args>(args)...);
        } else {
            *value = T{ std::forward<Args>(args)... };
        }
        return std::make_pair(std::move(value), is_emplaced);
    }

    [[nodiscard]] const auto& memory() const { return memory_; }

private:
    template <typename Self, bool IsConst = std::is_const_v<Self>>
    static maybe<detail::persistent<T, Alloc, IsConst>>
        lookup_impl(Self& self, const Id& item_id, std::size_t parent_recursion_limit) {
        if (const auto address_it = self.table_.find(item_id); address_it != self.table_.end()) {
            return detail::persistent<T, Alloc, IsConst>{ self.memory_, address_it.value() };
        }
        if (parent_recursion_limit == 0 || !self.get_parent_) {
            return null;
        }
        if (auto* parent = self.get_parent_()) {
            return lookup_impl(*parent, item_id, parent_recursion_limit - 1);
        }
        return null;
    }

    std::pair<value_type, bool> new_addr_impl(const Id& id) {
        const std::size_t new_address = memory_.size();
        const auto [address_it, is_address_emplaced] = table_.try_emplace(id, new_address);
        return std::make_pair(value_type{ memory_, address_it.value() }, is_address_emplaced);
    }

private:
    memory_type memory_;
    table_type table_;
    get_parent_type get_parent_;
};

} // namespace sl::meta
