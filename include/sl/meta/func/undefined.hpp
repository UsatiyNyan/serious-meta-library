//
// Created by usatiynyan.
//

#pragma once

#include <libassert/assert.hpp>

namespace sl::meta {

struct undefined {
    [[noreturn]] undefined() { PANIC(); }
};

} // namespace sl::meta
