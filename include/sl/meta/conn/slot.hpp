//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/func/function.hpp"

namespace sl::meta {

template <typename... Ts>
using slot = unique_function<void(Ts...)>;

} // namespace sl::meta
