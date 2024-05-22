//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/string/hash_view.hpp"

#include <tl/optional.hpp>
#include <tsl/robin_map.h>

#include <string>

namespace sl::meta {

template <
    typename C,
    template <typename> typename Traits = std::char_traits,
    template <typename> typename Alloc = std::allocator>
class unique_string {
public:
    unique_string(
        const std::basic_string<C, Traits<C>, Alloc<C>>& memory,
        std::size_t address,
        std::size_t size,
        std::size_t hash
    )
        : memory_{ memory }, address_{ address }, size_{ size }, hash_{ hash } {}

    [[nodiscard]] auto string_view() const {
        return std::basic_string_view<C, Traits<C>>{ &memory_.at(address_), size_ };
    }
    [[nodiscard]] auto hash() const { return hash_; }

    bool operator==(const unique_string&) const = default;
    bool operator==(const basic_hash_string_view<C>& other) const {
        return hash() == other.hash() && string_view() == other.string_view();
    }

private:
    const std::basic_string<C, Traits<C>, Alloc<C>>& memory_;
    std::size_t address_;
    std::size_t size_;
    std::size_t hash_;
};

template <
    typename C,
    template <typename> typename Traits = std::char_traits,
    template <typename> typename Alloc = std::allocator>
class unique_string_storage {
    struct cell {
        std::size_t address;
        std::size_t size;
    };
    class cell_table_hash;

public:
    using reference = unique_string<C, Traits, Alloc>;

public:
    explicit unique_string_storage(
        std::size_t capacity,
        tl::optional<const unique_string_storage&> parent = tl::nullopt
    )
        : memory_{}, cell_table_{ capacity }, parent_{ parent } {
        memory_.reserve(capacity);
    }

    [[nodiscard]] tl::optional<reference> lookup(
        const basic_hash_string_view<C>& str_id,
        std::size_t parent_recursion_limit = std::numeric_limits<std::size_t>::max()
    ) const {
        if (const auto cell_it = cell_table_.find(str_id); cell_it != cell_table_.end()) {
            return reference{ memory_, cell_it.value() };
        }
        if (parent_recursion_limit == 0) {
            return tl::nullopt;
        }
        return parent_.and_then([&str_id, &parent_recursion_limit](const unique_string_storage& p) {
            return p.lookup(str_id, parent_recursion_limit - 1);
        });
    }

    // Does not lookup in parents before inserting.
    template <typename Range>
    [[nodiscard]] std::pair<reference, bool> emplace(basic_hash_string_view<C> str_id) {
        if (auto maybe_reference = lookup(str_id, 0)) {
            return std::make_pair(std::move(maybe_reference).value(), false);
        }

        const auto sv = str_id.string_view();

        // careful, order of operations is important
        const cell new_cell{ .address = memory_.size(), .size = sv.size() };
        const std::size_t hash = str_id.hash();
        memory_.append(sv);
        const reference key_reference{ memory_, new_cell.address, new_cell.size, hash };
        const auto [cell_it, is_cell_emplaced] = cell_table_.try_emplace(key_reference, new_cell);

        // handle ambiguous case, should not ever happen
        if (!is_cell_emplaced) [[unlikely]] {
            memory_.erase(memory_.size() - sv.size());
            return std::make_pair(reference{ memory_, cell_it.value().address, cell_it.value().size, hash }, false);
        }

        return std::make_pair(key_reference, true);
    }

    [[nodiscard]] const auto& memory() const { return memory_; }

private:
    std::basic_string<C, Traits<C>, Alloc<C>> memory_;
    tsl::robin_map<reference, cell, cell_table_hash, std::equal_to<>, Alloc<std::pair<reference, cell>>> cell_table_;
    tl::optional<const unique_string_storage&> parent_;
};


template <typename C, template <typename> typename Traits, template <typename> typename Alloc>
class unique_string_storage<C, Traits, Alloc>::cell_table_hash {
    std::size_t operator()(const reference& r) { return r.hash(); }
    std::size_t operator()(const basic_hash_string_view<C>& r) { return r.hash(); }
};

} // namespace sl::meta

template <typename C, template <typename> typename Traits, template <typename> typename Alloc>
struct std::hash<sl::meta::unique_string<C, Traits, Alloc>> {
    std::size_t operator()(const sl::meta::unique_string<C, Traits, Alloc>& r) const { return r.hash(); }
};
