//
// Created by usatiynyan.
//

#pragma once

#include <tl/optional.hpp>

namespace sl::meta {

template <typename T>
using maybe = ::tl::optional<T>;

constexpr auto null = ::tl::nullopt;
constexpr auto in_place = ::tl::in_place;

} // namespace sl::meta
