//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/func/function.hpp"
#include "sl/meta/monad/maybe.hpp"
#include "sl/meta/traits/unique.hpp"
#include "sl/meta/type/const.hpp"

#include <range/v3/size.hpp>
#include <tsl/robin_map.h>

#include <vector>

namespace sl::meta {
namespace detail {

struct persistent_array_cell {
    std::size_t address;
    std::size_t size;

    bool operator==(const persistent_array_cell&) const = default;
};

template <typename T, template <typename> typename Alloc, bool IsConst>
class persistent_array {
    using memory_type = type::add_const_if_t<std::vector<T, Alloc<T>>, IsConst>;
    using deref_type = type::add_const_if_t<T, IsConst>;

public:
    persistent_array(memory_type& memory, persistent_array_cell cell) : cell_{ cell }, memory_{ &memory } {}

    [[nodiscard]] auto span() const { return std::span{ ptr(), cell_.size }; }
    [[nodiscard]] auto span() { return std::span{ ptr(), cell_.size }; }

private:
    deref_type* ptr() const { return memory_->data() + cell_.address; }

private:
    persistent_array_cell cell_;
    memory_type* memory_;
};

} // namespace detail

template <typename T, template <typename> typename Alloc = std::allocator>
using persistent_array = detail::persistent_array<T, Alloc, /*is_const=*/false>;

template <typename T, template <typename> typename Alloc = std::allocator>
using const_persistent_array = detail::persistent_array<T, Alloc, /*is_const=*/true>;

template <
    typename Id,
    typename T,
    template <typename> typename Hash = std::hash,
    template <typename> typename Equal = std::equal_to,
    template <typename> typename Alloc = std::allocator>
struct persistent_array_storage : immovable {
    using memory_alloc_type = Alloc<T>;
    using memory_type = std::vector<T, memory_alloc_type>;

    using value_type = persistent_array<T, Alloc>;
    using const_value_type = const_persistent_array<T, Alloc>;
    using cell_type = detail::persistent_array_cell;
    using table_alloc_type = Alloc<std::pair<Id, cell_type>>;
    using table_type = tsl::robin_map<Id, cell_type, Hash<Id>, Equal<Id>, table_alloc_type>;

    using get_parent_type = unique_function<persistent_array_storage*() const>;

    struct init_type {
        get_parent_type get_parent = {};
        std::size_t memory_capacity = 0;
        std::size_t table_capacity = 0;
        const memory_alloc_type& memory_alloc = {};
        const table_alloc_type& table_alloc = {};
    };

public:
    explicit persistent_array_storage(init_type init = {})
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
    // No exceptions pls, they are not handled.
    template <typename Range>
    [[nodiscard]] std::pair<value_type, bool> emplace(const Id& id, Range&& range) {
        const auto [cell_it, is_emplaced] = table_.try_emplace(
            id,
            cell_type{
                .address = memory_.size(),
                .size = ranges::size(range),
            }
        );
        value_type value{ memory_, cell_it.value() };
        if (is_emplaced) {
            memory_.insert(memory_.end(), ranges::begin(range), ranges::end(range));
        }
        return std::make_pair(std::move(value), is_emplaced);
    }

    [[nodiscard]] const auto& memory() const { return memory_; }

private:
    template <typename Self, bool IsConst = std::is_const_v<Self>>
    static maybe<detail::persistent_array<T, Alloc, IsConst>>
        lookup_impl(Self& self, const Id& item_id, std::size_t parent_recursion_limit) {
        if (const auto cell_it = self.table_.find(item_id); cell_it != self.table_.end()) {
            return detail::persistent_array<T, Alloc, IsConst>{ self.memory_, cell_it.value() };
        }
        if (parent_recursion_limit == 0 || !self.get_parent_) {
            return null;
        }
        if (auto* parent = self.get_parent_()) {
            return lookup_impl(*parent, item_id, parent_recursion_limit - 1);
        }
        return null;
    }

private:
    memory_type memory_;
    table_type table_;
    get_parent_type get_parent_;
};

} // namespace sl::meta
