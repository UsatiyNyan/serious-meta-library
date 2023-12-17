//
// Created by usatiynyan on 8/1/23.
//

#pragma once

#include "sl/meta/traits/count_fields.hpp"
#include "sl/meta/tuple/tie_as_tuple_generated.hpp"

namespace sl::meta {

template <typename AggregateT>
    requires std::is_aggregate_v<AggregateT>
auto tie_as_tuple(AggregateT& value) {
    return detail::tie_as_tuple(value, std::integral_constant<std::size_t, count_fields<AggregateT>()>{});
}

template <typename T>
using tie_as_tuple_t = decltype(tie_as_tuple(std::declval<T&>()));

} // namespace sl::meta
