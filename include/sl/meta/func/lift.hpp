//
// Created by usatiynyan.
//

#pragma once

#define SL_META_LIFT(f) [](auto&&... args) { return f(std::forward<decltype(args)>(args)...); }
