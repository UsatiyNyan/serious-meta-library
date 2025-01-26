//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/lifetime/immovable.hpp"
#include "sl/meta/string/hash_view.hpp"

#include <libassert/assert.hpp>
#include <tl/expected.hpp>
#include <tl/optional.hpp>
#include <tsl/robin_map.h>

#include <string>

namespace sl::meta {

template <
    typename C,
    template <typename> typename Traits = std::char_traits,
    template <typename> typename Alloc = std::allocator>
class basic_unique_string {
public:
    basic_unique_string(
        const std::basic_string<C, Traits<C>, Alloc<C>>& memory,
        std::size_t address,
        std::size_t size,
        std::size_t hash
    )
        : memory_{ &memory }, address_{ address }, size_{ size }, hash_{ hash } {}

    [[nodiscard]] auto string_view() const {
        return std::basic_string_view<C, Traits<C>>{ &memory_->at(address_), size_ };
    }
    [[nodiscard]] auto hash() const { return hash_; }

    bool operator==(const basic_unique_string& other) const = default;
    bool operator==(const basic_hash_string_view<C>& other) const {
        return hash() == other.hash() && string_view() == other.string_view();
    }

private:
    const std::basic_string<C, Traits<C>, Alloc<C>>* memory_;
    std::size_t address_;
    std::size_t size_;
    std::size_t hash_;
};

template <typename C, template <typename> typename Traits, template <typename> typename Alloc>
bool operator==(const basic_hash_string_view<C>& a, const basic_unique_string<C, Traits, Alloc>& b) {
    return b == a;
}

template <
    typename C,
    template <typename> typename Traits = std::char_traits,
    template <typename> typename Alloc = std::allocator>
class basic_unique_string_storage : public immovable {
    struct cell {
        std::size_t address;
        std::size_t size;
    };
    struct cell_table_hash;

public:
    using reference = basic_unique_string<C, Traits, Alloc>;

public:
    explicit basic_unique_string_storage(
        std::size_t capacity,
        tl::optional<const basic_unique_string_storage&> parent = tl::nullopt,
        const Alloc<C>& memory_alloc = {},
        const Alloc<std::pair<reference, cell>>& table_alloc = {}
    )
        : memory_{ memory_alloc }, cell_table_{ capacity, table_alloc }, parent_{ parent } {
        memory_.reserve(capacity);
    }

    [[nodiscard]] tl::optional<reference> lookup(
        const basic_hash_string_view<C>& str_id,
        std::size_t parent_recursion_limit = std::numeric_limits<std::size_t>::max()
    ) const {
        if (const auto cell_it = cell_table_.find(str_id); cell_it != cell_table_.end()) {
            return reference{ memory_, cell_it.value().address, cell_it.value().size, str_id.hash() };
        }
        if (parent_recursion_limit == 0) {
            return tl::nullopt;
        }
        return parent_.and_then([&str_id, &parent_recursion_limit](const basic_unique_string_storage& p) {
            return p.lookup(str_id, parent_recursion_limit - 1);
        });
    }

    // Does not lookup in parents before inserting.
    // returns: pair<reference, true if inserted|false if assigned>
    [[nodiscard]] tl::expected<reference, reference> emplace(basic_hash_string_view<C> str_id) {
        if (auto maybe_reference = lookup(str_id, 0)) {
            return tl::make_unexpected(std::move(maybe_reference).value());
        }

        const auto sv = str_id.string_view();

        // careful, order of operations is important
        const cell new_cell{ .address = memory_.size(), .size = sv.size() };
        const std::size_t hash = str_id.hash();
        memory_.append(sv);
        const reference key_reference{ memory_, new_cell.address, new_cell.size, hash };
        const auto [cell_it, is_cell_emplaced] = cell_table_.try_emplace(key_reference, new_cell);
        const auto& cell = cell_it.value();
        DEBUG_ASSERT(
            is_cell_emplaced, "cell was not found in lookup previously, should not ever happen", sv, new_cell, cell
        );

        // handle ambiguous case
        if (!is_cell_emplaced) [[unlikely]] {
            memory_.erase(memory_.size() - sv.size());
            return tl::make_unexpected(reference{ memory_, cell.address, cell.size, hash });
        }

        return key_reference;
    }

    [[nodiscard]] const auto& memory() const { return memory_; }

private:
    std::basic_string<C, Traits<C>, Alloc<C>> memory_;
    tsl::robin_map<reference, cell, cell_table_hash, std::equal_to<>, Alloc<std::pair<reference, cell>>> cell_table_;
    tl::optional<const basic_unique_string_storage&> parent_;
};


template <typename C, template <typename> typename Traits, template <typename> typename Alloc>
struct basic_unique_string_storage<C, Traits, Alloc>::cell_table_hash {
    std::size_t operator()(const reference& r) const { return r.hash(); }
    std::size_t operator()(const basic_hash_string_view<C>& r) const { return r.hash(); }
};

using unique_string = basic_unique_string<char>;
using unique_string_storage = basic_unique_string_storage<char>;

constexpr auto operator""_us(const char* str, std::size_t len) {
    return [hsv = hash_string_view{ std::string_view{ str, len } }](unique_string_storage& storage) {
        auto result = storage.emplace(hsv);
        return result.has_value() ? result.value() : result.error();
    };
}

} // namespace sl::meta

template <typename C, template <typename> typename Traits, template <typename> typename Alloc>
struct std::hash<sl::meta::basic_unique_string<C, Traits, Alloc>> {
    std::size_t operator()(const sl::meta::basic_unique_string<C, Traits, Alloc>& r) const { return r.hash(); }
};
