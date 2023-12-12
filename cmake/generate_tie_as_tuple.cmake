function(sl_generate_tie_as_tuple n_fields)
    set(generated_code "")
    foreach (i RANGE 1 ${n_fields})
        set(generated_fields)

        foreach (j RANGE 1 ${i})
            list(APPEND generated_fields "f${j}")
        endforeach ()

        list(JOIN generated_fields ", " generated_fields)

        set(generated_function
"template <typename T>
auto tie_as_tuple(T& value, std::integral_constant<std::size_t, ${i}>) {
    auto& [${generated_fields}] = value;
    return std::tie(${generated_fields});
}")
        set(generated_code "${generated_code}\n\n${generated_function}")
    endforeach ()

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/tie_as_tuple_generated.in"
            "${CMAKE_CURRENT_BINARY_DIR}/include/sl/meta/tuple/tie_as_tuple_generated.hpp"
            @ONLY)
endfunction()
