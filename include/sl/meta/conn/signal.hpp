//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/conn/slot.hpp"
#include "sl/meta/func/unit.hpp"

#include <list>

namespace sl::meta {

template <typename T = unit>
class signal {
public:
    using connection = typename std::list<slot<T>>::iterator;

    void operator()(const T& value) noexcept {
        for (auto& slot_item : slots_) {
            slot_item(value);
        }
    }

    template <typename F>
    connection connect(F&& f) {
        slots_.emplace_back(std::forward<F>(f));
        return std::prev(slots_.end());
    }

    void disconnect(connection c) { slots_.erase(c); }

private:
    std::list<slot<T>> slots_;
};

} // namespace sl::meta
