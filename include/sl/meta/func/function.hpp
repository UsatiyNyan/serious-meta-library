//
// Created by usatiynyan.
//

#pragma once

#include <function2/function2.hpp>

namespace sl::meta {

template <typename Signature, typename Capacity = fu2::capacity_default>
using unique_function = fu2::function_base<
    /*IsOwning=*/true,
    /*IsCopyable=*/false,
    /*Capacity=*/Capacity,
    /*IsThrowing=*/false,
    /*HasStrongExceptGuarantee=*/true,
    /*Signatures=*/Signature>;

} // namespace sl::meta
