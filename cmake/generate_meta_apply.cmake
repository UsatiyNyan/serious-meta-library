function(sl_generate_meta_apply n_fields)
    set(generated_code "")
    foreach (i RANGE 1 ${n_fields})
        set(generated_fields)
        set(generated_moved_fields)

        math(EXPR i_minus_1 "${i} - 1")
        foreach (j RANGE 0 ${i_minus_1})
            list(APPEND generated_fields "f${j}")
            list(APPEND generated_moved_fields "std::move(f${j})")
        endforeach ()

        list(JOIN generated_fields ", " generated_fields)
        list(JOIN generated_moved_fields ", " generated_moved_fields)

        set(generated_function
"template <typename T, typename F>
auto apply(F&& func, T&& value, std::integral_constant<std::size_t, ${i}>) {
    if constexpr (std::is_lvalue_reference_v<T&&>) {
        auto [${generated_fields}] = value;
        return std::forward<F>(func)(${generated_fields});
    } else if constexpr (std::is_rvalue_reference_v<T&&>) {
        auto [${generated_fields}] = std::move(value);
        return std::forward<F>(func)(${generated_moved_fields});
    }
}")
        set(generated_code "${generated_code}\n\n${generated_function}")
    endforeach ()

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/apply_generated.in"
            "${CMAKE_CURRENT_BINARY_DIR}/include/sl/meta/tuple/apply_generated.hpp"
            @ONLY)
endfunction()
