//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/conn/slot.hpp"

#include <list>

namespace sl::meta {

template <typename... Ts>
class signal {
public:
    using connection = typename std::list<slot<Ts...>>::iterator;

    void operator()(const Ts&... values) noexcept {
        for (auto& slot_item : slots_) {
            slot_item(values...);
        }
    }

    template <typename F>
    connection connect(F&& f) {
        slots_.emplace_back(std::forward<F>(f));
        return std::prev(slots_.end());
    }

    void disconnect(connection c) { slots_.erase(c); }

private:
    std::list<slot<Ts...>> slots_;
};

} // namespace sl::meta
