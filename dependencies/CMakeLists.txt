cpmaddpackage(
        NAME function2
        URL https://github.com/Naios/function2/archive/refs/tags/4.2.4.tar.gz)
cpmaddpackage(
        NAME range-v3
        URL https://github.com/ericniebler/range-v3/archive/refs/tags/0.12.0.tar.gz)
cpmaddpackage(
        NAME tsl-robin-map
        URL https://github.com/Tessil/robin-map/archive/refs/tags/v1.4.1.tar.gz)
cpmaddpackage(
        NAME tl-optional
        URL https://github.com/TartanLlama/optional/archive/refs/tags/v1.1.0.tar.gz
        OPTIONS
        "OPTIONAL_BUILD_TESTS OFF"
        "OPTIONAL_BUILD_PACKAGE OFF"
        )
cpmaddpackage(
        NAME tl-expected
        URL https://github.com/TartanLlama/expected/archive/refs/tags/v1.1.0.tar.gz
        OPTIONS
        "EXPECTED_BUILD_TESTS OFF"
        "EXPECTED_BUILD_PACKAGE OFF"
        )
cpmaddpackage(
        NAME fmt
        URL https://github.com/fmtlib/fmt/archive/refs/tags/12.1.0.tar.gz)

sl_target_link_system_libraries(${PROJECT_NAME}
        INTERFACE
        function2::function2
        tl::optional
        tl::expected
        range-v3::range-v3
        tsl::robin_map
        fmt::fmt
)
