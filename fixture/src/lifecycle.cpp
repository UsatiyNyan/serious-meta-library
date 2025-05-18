//
// Created by usatiynyan.
//

#include "sl/meta/fixture/lifecycle.hpp"
#include <iostream>

namespace sl::meta::fixture {

std::unordered_map<std::string, std::vector<lifecycle::state>> lifecycle::states;

void lifecycle::print_states() {
    for (const auto& [id, id_states] : lifecycle::states) {
        std::cout << id << ":\n";
        for (const auto& s : id_states) {
            std::cout << "  " << enum_to_str(s) << "\n";
        }
        std::cout << std::endl;
    }
}

lifecycle::lifecycle(std::string a_id) : id{ std::move(a_id) } { lifecycle::states[id].push_back(state::constructed); }

lifecycle::lifecycle(const lifecycle& other) : id{ copied_id(other.id) } {
    lifecycle::states[other.id].push_back(state::copy_constructed_from);
    lifecycle::states[id].push_back(state::copy_constructed);
}

lifecycle& lifecycle::operator=(const lifecycle& other) {
    lifecycle::states[other.id].push_back(state::copy_assigned_from);
    lifecycle::states[id].push_back(state::copy_assigned);
    return *this;
}

lifecycle::lifecycle(lifecycle&& other) : id{ moved_id(other.id) } {
    lifecycle::states[other.id].push_back(state::move_constructed_from);
    lifecycle::states[id].push_back(state::move_constructed);
}

lifecycle& lifecycle::operator=(lifecycle&& other) {
    lifecycle::states[other.id].push_back(state::move_assigned_from);
    lifecycle::states[id].push_back(state::move_assigned);
    return *this;
}

lifecycle::~lifecycle() { lifecycle::states[id].push_back(state::destructed); }

} // namespace sl::meta::fixture
