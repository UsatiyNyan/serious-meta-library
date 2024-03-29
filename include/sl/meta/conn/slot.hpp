//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/func/unit.hpp"

#include <function2/function2.hpp>

namespace sl::meta {

template <typename T = unit>
using slot = fu2::function_base<
    /*IsOwning=*/true,
    /*IsCopyable=*/false,
    /*Capacity=*/fu2::capacity_default,
    /*IsThrowing=*/false,
    /*HasStrongExceptGuarantee=*/true,
    /*Signatures=*/void(const T&)>;

} // namespace sl::meta
