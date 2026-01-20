//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/assert.hpp"
#include "sl/meta/func/function.hpp"
#include "sl/meta/hash/string_view.hpp"
#include "sl/meta/monad/maybe.hpp"
#include "sl/meta/traits/unique.hpp"

#include <tsl/robin_map.h>

#include <cstddef>
#include <string_view>
#include <vector>

namespace sl::meta {
namespace detail {

struct basic_unique_string_cell {
    std::size_t address;
    std::size_t size;

    bool operator==(const basic_unique_string_cell&) const = default;
};

} // namespace detail

template <
    typename C,
    template <typename> typename Traits = std::char_traits,
    template <typename> typename Alloc = std::allocator>
struct basic_unique_string {
    basic_unique_string(const std::vector<C, Alloc<C>>& memory, detail::basic_unique_string_cell cell, std::size_t hash)
        : memory_{ &memory }, cell_{ cell }, hash_{ hash } {}

    [[nodiscard]] auto string_view() const {
        return std::basic_string_view<C, Traits<C>>{ memory_->data() + cell_.address, cell_.size };
    }
    [[nodiscard]] auto hash() const { return hash_; }

    bool operator==(const basic_unique_string&) const = default;
    bool operator==(const basic_hash_string_view<C>& other) const {
        return hash() == other.hash() && string_view() == other.string_view();
    }

private:
    const std::vector<C, Alloc<C>>* memory_;
    detail::basic_unique_string_cell cell_;
    std::size_t hash_;
};

template <typename C, template <typename> typename Traits, template <typename> typename Alloc>
bool operator==(const basic_hash_string_view<C>& a, const basic_unique_string<C, Traits, Alloc>& b) {
    return b == a;
}

namespace detail {

struct basic_unique_string_cell_table_hash {
    template <typename C, template <typename> typename Traits, template <typename> typename Alloc>
    std::size_t operator()(const basic_unique_string<C, Traits, Alloc>& us) const {
        return us.hash();
    }

    template <typename C>
    std::size_t operator()(const basic_hash_string_view<C>& hsv) const {
        return hsv.hash();
    }
};

} // namespace detail

template <
    typename C,
    template <typename> typename Traits = std::char_traits,
    template <typename> typename Alloc = std::allocator>
struct basic_unique_string_storage : immovable {
    using memory_alloc_type = Alloc<C>;
    using memory_type = std::vector<C, memory_alloc_type>;

    using value_type = basic_unique_string<C, Traits, Alloc>;
    using cell_type = detail::basic_unique_string_cell;
    using hash_type = detail::basic_unique_string_cell_table_hash;
    using table_alloc_type = Alloc<std::pair<value_type, cell_type>>;
    using table_type = tsl::robin_map<
        /*Key=*/value_type,
        /*T=*/cell_type,
        /*Hash=*/hash_type,
        /*KeyEqual=*/std::equal_to<>,
        /*Allocator=*/table_alloc_type>;

    using get_parent_type = unique_function<const basic_unique_string_storage*() const>;

    struct init_type {
        get_parent_type get_parent = {};
        std::size_t memory_capacity = 0;
        std::size_t table_capacity = 0;
        const memory_alloc_type& memory_alloc = {};
        const table_alloc_type& table_alloc = {};
    };

public:
    explicit basic_unique_string_storage(init_type init = {})
        : memory_{ init.memory_alloc }, table_{ init.table_capacity, init.table_alloc },
          get_parent_{ std::move(init.get_parent) } {
        memory_.reserve(init.memory_capacity);
    }

    [[nodiscard]] maybe<value_type> lookup(
        basic_hash_string_view<C> str_id,
        std::size_t parent_recursion_limit = std::numeric_limits<std::size_t>::max()
    ) const {
        if (const auto cell_it = table_.find(str_id); cell_it != table_.end()) {
            return value_type{ memory_, cell_it.value(), str_id.hash() };
        }
        if (parent_recursion_limit == 0 || !get_parent_) {
            return null;
        }
        if (auto* parent = get_parent_()) {
            return parent->lookup(str_id, parent_recursion_limit - 1);
        }
        return null;
    }

    [[nodiscard]] maybe<value_type> lookup(
        std::basic_string_view<C> str,
        std::size_t parent_recursion_limit = std::numeric_limits<std::size_t>::max()
    ) const {
        return lookup(basic_hash_string_view<C>{ str }, parent_recursion_limit);
    }

    [[nodiscard]] std::pair<value_type, bool>
        emplace(basic_hash_string_view<C> str_id, std::size_t parent_recursion_limit = 0) {
        if (auto maybe_value = lookup(str_id, parent_recursion_limit)) {
            return std::make_pair(std::move(maybe_value).value(), false);
        }

        const auto sv = str_id.string_view();
        const std::size_t hash = str_id.hash();

        // careful, order of operations is important
        cell_type cell{
            .address = memory_.size(),
            .size = sv.size(),
        };
        memory_.insert(memory_.end(), sv.begin(), sv.end());
        value_type value{ memory_, cell, hash };
        const auto [cell_it, is_cell_emplaced] = table_.try_emplace(value, cell);

        // handle ambiguous case
        if (!ASSERT_VAL(is_cell_emplaced, "cell was not found in lookup previously, should not ever happen", sv)) {
            memory_.erase(std::prev(memory_.end(), static_cast<std::ptrdiff_t>(sv.size())), memory_.end());
            return std::make_pair(value_type{ memory_, cell, hash }, false);
        }

        return std::make_pair(std::move(value), true);
    }

    [[nodiscard]] std::pair<value_type, bool>
        emplace(std::basic_string_view<C> str, std::size_t parent_recursion_limit = 0) {
        return emplace(basic_hash_string_view<C>{ str }, parent_recursion_limit);
    }

    [[nodiscard]] value_type insert(basic_hash_string_view<C> str_id, std::size_t parent_recursion_limit = 0) {
        return emplace(str_id, parent_recursion_limit).first;
    }

    [[nodiscard]] value_type insert(std::basic_string_view<C> str, std::size_t parent_recursion_limit = 0) {
        return emplace(str, parent_recursion_limit).first;
    }

    [[nodiscard]] const auto& memory() const { return memory_; }
    [[nodiscard]] auto memory_str() const { return std::string_view{ memory_.data(), memory_.size() }; }

private:
    memory_type memory_;
    table_type table_;
    get_parent_type get_parent_;
};


} // namespace sl::meta

template <typename C, template <typename> typename Traits, template <typename> typename Alloc>
struct std::hash<sl::meta::basic_unique_string<C, Traits, Alloc>> {
    std::size_t operator()(const sl::meta::basic_unique_string<C, Traits, Alloc>& r) const { return r.hash(); }
};
