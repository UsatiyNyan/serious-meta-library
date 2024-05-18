//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/lifetime/unique.hpp"

#include <type_traits>
#include <utility>

namespace sl::meta {

template <typename F, typename T = std::invoke_result_t<F>>
class lazy_eval : public unique {
public:
    explicit lazy_eval(F f) : f_{ std::move(f) } {}

    operator T() && { return f_(); }

private:
    F f_;
};

} // namespace sl::meta
