//
// Created by usatiynyan.
//

#pragma once

#include <utility>

namespace sl::meta {

#define lift(f) [](auto&&... args) { return f(std::forward<decltype(args)>(args)...); }

} // namespace sl::meta
