//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/conn/signal.hpp"
#include "sl/meta/lifetime/finalizer.hpp"


namespace sl::meta {

template <typename T>
class scoped_conn : public finalizer<scoped_conn<T>> {
public:
    template <typename F>
    scoped_conn(signal<T>& s, F&& f)
        : finalizer<scoped_conn>{ [](scoped_conn& self) { self.signal_.get().disconnect(std::move(self.conn_)); } },
          signal_{ s }, conn_{ s.connect(std::forward<F>(f)) } {}

private:
    std::reference_wrapper<signal<T>> signal_;
    typename signal<T>::connection conn_;
};

} // namespace sl::meta
