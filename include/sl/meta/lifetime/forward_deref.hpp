//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/traits/treat_as.hpp"

#include <type_traits>

namespace sl::meta {

template <treat_as_ptr PtrT>
constexpr auto forward_deref(PtrT& ptr) {
    if constexpr (std::is_lvalue_reference_v<PtrT>) {
        return *ptr;
    } else {
        return std::move(*ptr);
    }
}
} // namespace sl::meta
