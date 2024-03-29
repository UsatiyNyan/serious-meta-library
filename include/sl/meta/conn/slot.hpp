//
// Created by usatiynyan.
//

#pragma once

#include <function2/function2.hpp>

namespace sl::meta {

template <typename... Ts>
using slot = fu2::function_base<
    /*IsOwning=*/true,
    /*IsCopyable=*/false,
    /*Capacity=*/fu2::capacity_default,
    /*IsThrowing=*/false,
    /*HasStrongExceptGuarantee=*/true,
    /*Signatures=*/void(const Ts&...)>;

} // namespace sl::meta
