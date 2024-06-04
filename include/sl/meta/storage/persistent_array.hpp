//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/func/const.hpp"
#include "sl/meta/lifetime/immovable.hpp"

#include <range/v3/size.hpp>
#include <tl/optional.hpp>
#include <tsl/robin_map.h>

#include <vector>

namespace sl::meta {
namespace detail {

template <typename T, template <typename> typename Alloc, bool is_const>
class persistent_array {
    using memory_t = add_const_if_t<std::vector<T, Alloc<T>>, is_const>;
    using span_t = std::span<add_const_if_t<T, is_const>>;

public:
    persistent_array(memory_t& memory, std::size_t address, std::size_t size)
        : memory_{ &memory }, address_{ address }, size_{ size } {}

    [[nodiscard]] auto span() const { return span_t{ &memory_->at(address_), size_ }; }
    [[nodiscard]] auto span() { return span_t{ &memory_->at(address_), size_ }; }

private:
    memory_t* memory_;
    std::size_t address_;
    std::size_t size_;
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
class persistent_array_storage : public immovable {
    struct cell {
        std::size_t address;
        std::size_t size;
    };

public:
    explicit persistent_array_storage(
        std::size_t capacity,
        tl::optional<persistent_array_storage&> parent = tl::nullopt
    )
        : memory_{}, cell_table_{ capacity }, parent_{ parent } {
        memory_.reserve(capacity);
    }

    [[nodiscard]] tl::optional<const_persistent_array<T, Alloc>>
        lookup(const Id& item_id, std::size_t parent_recursion_limit = std::numeric_limits<std::size_t>::max()) const {
        return lookup_impl(*this, item_id, parent_recursion_limit);
    }

    [[nodiscard]] tl::optional<persistent_array<T, Alloc>>
        lookup(const Id& item_id, std::size_t parent_recursion_limit = std::numeric_limits<std::size_t>::max()) {
        return lookup_impl(*this, item_id, parent_recursion_limit);
    }

    // Does not lookup in parents before inserting.
    // No exceptions pls, they are not handled.
    template <typename Range>
    [[nodiscard]] std::pair<persistent_array<T, Alloc>, bool> emplace(const Id& id, Range&& range) {
        const auto [cell_it, is_cell_emplaced] = cell_table_.try_emplace(
            id,
            cell{
                .address = memory_.size(),
                .size = ranges::size(range),
            }
        );
        const cell& cell = cell_it.value();
        auto result = std::make_pair(persistent_array<T, Alloc>{ memory_, cell.address, cell.size }, is_cell_emplaced);
        if (is_cell_emplaced) {
            memory_.insert(memory_.end(), ranges::begin(range), ranges::end(range));
        }
        return result;
    }

    [[nodiscard]] const auto& memory() const { return memory_; }

private:
    template <typename Self, bool is_const = std::is_const_v<Self>>
    static tl::optional<detail::persistent_array<T, Alloc, is_const>>
        lookup_impl(Self& self, const Id& item_id, std::size_t parent_recursion_limit) {
        if (const auto cell_it = self.cell_table_.find(item_id); cell_it != self.cell_table_.end()) {
            const cell& cell = cell_it.value();
            return detail::persistent_array<T, Alloc, is_const>{ self.memory_, cell.address, cell.size };
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
    tsl::robin_map<Id, cell, Hash<Id>, Equal<Id>, Alloc<std::pair<Id, cell>>> cell_table_;
    tl::optional<persistent_array_storage&> parent_;
};

} // namespace sl::meta
