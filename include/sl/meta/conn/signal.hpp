//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/func/function.hpp"
#include "sl/meta/lifetime/finalizer.hpp"
#include "sl/meta/traits/unique.hpp"

#include <list>

namespace sl::meta {

template <typename... Ts>
struct signal : public unique {
    using slot = unique_function<void(Ts...)>;
    using connection = typename std::list<slot>::iterator;

    void operator()(const Ts&... values) noexcept {
        for (auto& slot_item : slots_) {
            slot_item(values...);
        }
    }

    template <typename F>
    [[nodiscard]] connection connect(F&& f) {
        slots_.emplace_back(std::forward<F>(f));
        return std::prev(slots_.end());
    }

    void disconnect(connection c) { slots_.erase(c); }

private:
    std::list<slot> slots_;
};

template <typename... Ts>
struct scoped_conn : finalizer<scoped_conn<Ts...>> {
    template <typename F>
    scoped_conn(signal<Ts...>& s, F&& f)
        : finalizer<scoped_conn>{ [](scoped_conn& self) { self.signal_.get().disconnect(std::move(self.conn_)); } },
          signal_{ s }, conn_{ s.connect(std::forward<F>(f)) } {}

private:
    std::reference_wrapper<signal<Ts...>> signal_;
    typename signal<Ts...>::connection conn_;
};

} // namespace sl::meta
