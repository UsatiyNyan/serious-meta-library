//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/monad/result.hpp"

#include <system_error>

namespace sl::meta {

inline std::error_code errno_code() { return std::make_error_code(std::errc{ errno }); }
inline auto errno_err() { return err(errno_code()); }

} // namespace sl::meta
