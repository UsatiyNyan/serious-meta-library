//
// Created by usatiynyan.
//

#pragma once

#include <range/v3/view/take_exactly.hpp>
#include <tl/optional.hpp>
#include <tsl/robin_map.h>

#include <vector>

namespace sl::meta {

template <typename T, template <typename> typename Alloc = std::allocator>
class persistent_array {
public:
    persistent_array(const std::vector<T, Alloc<T>>& memory, std::size_t address, std::size_t size)
        : memory_{ memory }, address_{ address }, size_{ size } {}

    [[nodiscard]] auto span() const { return std::span<T>{ &memory_->at(address_), size_ }; }

private:
    const std::vector<T, Alloc<T>>& memory_;
    std::size_t address_;
    std::size_t size_;
};

template <
    typename Id,
    typename T,
    template <typename> typename Hash = std::hash,
    template <typename> typename Equal = std::equal_to,
    template <typename> typename Alloc = std::allocator>
class persistent_array_storage {
    struct cell {
        std::size_t address;
        std::size_t size;
    };

public:
    using reference = persistent_array<T, Alloc>;

public:
    explicit persistent_array_storage(
        std::size_t capacity,
        tl::optional<const persistent_array_storage&> parent = tl::nullopt
    )
        : memory_{}, cell_table_{ capacity }, parent_{ parent } {
        memory_.reserve(capacity);
    }

    [[nodiscard]] tl::optional<reference>
        lookup(const Id& item_id, std::size_t parent_recursion_limit = std::numeric_limits<std::size_t>::max()) const {
        if (const auto cell_it = cell_table_.find(item_id); cell_it != cell_table_.end()) {
            return reference{ memory_, cell_it.value() };
        }
        if (parent_recursion_limit == 0) {
            return tl::nullopt;
        }
        return parent_.and_then([&item_id, &parent_recursion_limit](const persistent_array_storage& p) {
            return p.lookup(item_id, parent_recursion_limit - 1);
        });
    }

    // Does not lookup in parents before inserting.
    // No exceptions pls, they are not handled.
    template <typename Range>
    [[nodiscard]] std::pair<reference, bool> emplace(const Id& id, std::size_t size, Range&& range) {
        const cell new_cell{ .address = memory_.size(), .size = size };
        const auto [cell_it, is_cell_emplaced] = cell_table_.try_emplace(id, new_cell);
        auto result = std::make_pair(reference{ memory_, cell_it.value() }, is_cell_emplaced);
        if (is_cell_emplaced) {
            const auto exact_range = ranges::views::take_exactly(std::forward<Range>(range), size);
            memory_.insert(memory_.end(), ranges::begin(exact_range), ranges::end(exact_range));
        }
        return result;
    }

    [[nodiscard]] const auto& memory() const { return memory_; }

private:
    std::vector<T, Alloc<T>> memory_;
    tsl::robin_map<Id, cell, Hash<Id>, Equal<Id>, Alloc<std::pair<Id, cell>>> cell_table_;
    tl::optional<const persistent_array_storage&> parent_;
};

} // namespace sl::meta
