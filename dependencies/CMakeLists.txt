cpmaddpackage(
        NAME function2
        GIT_REPOSITORY "git@github.com:Naios/function2.git"
        GIT_TAG 4.2.4)
cpmaddpackage(
        NAME range-v3
        GIT_REPOSITORY "git@github.com:ericniebler/range-v3.git"
        GIT_TAG 0.12.0)
cpmaddpackage(
        NAME tsl-robin-map
        GIT_REPOSITORY "git@github.com:Tessil/robin-map"
        GIT_TAG v1.4.1)
cpmaddpackage(
        NAME tl-optional
        GIT_REPOSITORY "git@github.com:TartanLlama/optional.git"
        GIT_TAG v1.1.0
        OPTIONS
        "OPTIONAL_BUILD_TESTS OFF"
        "OPTIONAL_BUILD_PACKAGE OFF"
        )
cpmaddpackage(
        NAME tl-expected
        GIT_REPOSITORY "git@github.com:TartanLlama/expected.git"
        GIT_TAG v1.1.0
        OPTIONS
        "EXPECTED_BUILD_TESTS OFF"
        "EXPECTED_BUILD_PACKAGE OFF"
        )
cpmaddpackage(
        NAME fmt
        GIT_REPOSITORY "git@github.com:fmtlib/fmt.git"
        GIT_TAG 12.1.0)

sl_target_link_system_libraries(${PROJECT_NAME}
        INTERFACE
        function2::function2
        tl::optional
        tl::expected
        range-v3::range-v3
        tsl::robin_map
        fmt::fmt
)
