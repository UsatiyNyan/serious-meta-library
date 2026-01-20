#pragma once

#include <cstdlib>
#include <iostream>
#include <utility>

namespace sl::meta::detail {

inline void print_err(auto&& arg) { std::cerr << "\n" << arg << std::endl; }

inline void print_err(auto&& arg, auto&&... args) {
    std::cerr << "\n" << arg;
    print_err(args...);
}

} // namespace sl::meta::detail

#define SL_ASSERT_EXPAND(EXPR, WHAT, UNLIKELY, FILE, LINE, ...)                                           \
    do {                                                                                                  \
        if (!static_cast<bool>(EXPR)) UNLIKELY {                                                          \
                ::sl::meta::detail::print_err(FILE ":" #LINE, __func__, #WHAT "(" #EXPR ")" __VA_ARGS__); \
                std::abort();                                                                             \
            }                                                                                             \
    } while (false)

#define SL_ASSERT(EXPR, WHAT, UNLIKELY, FILE, LINE, ...) SL_ASSERT_EXPAND(EXPR, WHAT, UNLIKELY, FILE, LINE, __VA_ARGS__)

#define SL_ASSERT_VAL_EXPAND(EXPR, WHAT, UNLIKELY, FILE, LINE, ...)                                                 \
    [&, assert_expr_##LINE = (EXPR), assert_func_##LINE = __func__]() mutable {                                     \
        if (!static_cast<bool>(assert_expr_##LINE)) UNLIKELY {                                                      \
                ::sl::meta::detail::print_err(FILE ":" #LINE, assert_func_##LINE, #WHAT "(" #EXPR ")" __VA_ARGS__); \
                std::abort();                                                                                       \
            }                                                                                                       \
        return std::move(assert_expr_##LINE);                                                                       \
    }()

#define SL_ASSERT_VAL(EXPR, WHAT, UNLIKELY, FILE, LINE, ...) \
    SL_ASSERT_VAL_EXPAND(EXPR, WHAT, UNLIKELY, FILE, LINE, __VA_ARGS__)

#define ASSERT(EXPR, ...) SL_ASSERT(EXPR, ASSERT, [[unlikely]], __FILE__, __LINE__, __VA_ARGS__)
#define ASSERT_VAL(EXPR, ...) SL_ASSERT_VAL(EXPR, ASSERT, [[unlikely]], __FILE__, __LINE__, __VA_ARGS__)

#define PANIC(...) SL_ASSERT(false, PANIC, [[unlikely]], __FILE__, __LINE__, __VA_ARGS__)

#ifdef NDEBUG
#define DEBUG_ASSERT(...) (void)0;
#define DEBUG_ASSERT_VAL(EXPR, ...) EXPR
#else
#define DEBUG_ASSERT(EXPR, ...) SL_ASSERT(EXPR, DEBUG_ASSERT, , __FILE__, __LINE__, __VA_ARGS__)
#define DEBUG_ASSERT_VAL(EXPR, ...) SL_ASSERT_VAL(EXPR, DEBUG_ASSERT, , __FILE__, __LINE__, __VA_ARGS__)
#endif
