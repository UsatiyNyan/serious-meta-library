//
// Created by usatiynyan on 8/1/23.
//

#pragma once

#include "sl/meta/traits/count_fields.hpp"
#include "sl/meta/tuple/tie_as_tuple_generated.hpp"

namespace sl::meta {

// tie any aggregate strucutre as tuple
template <typename T>
auto tie_as_tuple(T& value) {
    return detail::tie_as_tuple(value, std::integral_constant<std::size_t, count_fields<T>()>{});
}

} // namespace sl::meta
