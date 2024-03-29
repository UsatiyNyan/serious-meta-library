//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/conn/slot.hpp"
#include "sl/meta/func/unit.hpp"

#include <forward_list>

namespace sl::meta {

template <typename T = unit>
class signal {
public:
    using connection = typename std::forward_list<slot<T>>::iterator;

    void operator()(const T& value) noexcept {
        for (auto& slot_item : slots_) {
            slot_item(value);
        }
    }

    template <typename F>
    connection connect(F&& f) {
        slots_.emplace_front(std::forward<F>(f));
        return slots_.before_begin();
    }

    void disconnect(connection c) { slots_.erase_after(c); }

private:
    std::forward_list<slot<T>> slots_;
};

} // namespace sl::meta
