//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/lifetime/defer.hpp"

#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>


namespace sl::meta::fixture {

struct lifecycle {
    enum class state {
        constructed,
        copy_constructed,
        copy_constructed_from,
        copy_assigned,
        copy_assigned_from,
        move_constructed,
        move_constructed_from,
        move_assigned,
        move_assigned_from,
        destructed,
    };

    static constexpr std::string_view enum_to_str(state s) {
        switch (s) {
        case state::constructed:
            return "constructed";
        case state::copy_constructed:
            return "copy_constructed";
        case state::copy_constructed_from:
            return "copy_constructed_from";
        case state::copy_assigned:
            return "copy_assigned";
        case state::copy_assigned_from:
            return "copy_assigned_from";
        case state::move_constructed:
            return "move_constructed";
        case state::move_constructed_from:
            return "move_constructed_from";
        case state::move_assigned:
            return "move_assigned";
        case state::move_assigned_from:
            return "move_assigned_from";
        case state::destructed:
            return "destructed";
        }
        assert(false);
    }

    // ew unordered_map
    // TODO: LRU
    static std::unordered_map<std::string, std::vector<state>> states;
    static auto make_states_guard() {
        assert(states.empty());
        return defer{ [] { states.clear(); } };
    }
    static void print_states();

    explicit lifecycle(std::string id);
    lifecycle(const lifecycle& other);
    lifecycle& operator=(const lifecycle& other);
    lifecycle(lifecycle&& other);
    lifecycle& operator=(lifecycle&& other);
    ~lifecycle();

    std::string id;
};

} // namespace sl::meta::fixture
