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
        GIT_TAG v1.3.0)
cpmaddpackage(
        NAME libassert
        GIT_REPOSITORY "git@github.com:jeremy-rifkin/libassert.git"
        GIT_TAG v2.0.1)

include("${CMAKE_CURRENT_SOURCE_DIR}/tl.cmake")

sl_target_link_system_libraries(${PROJECT_NAME}
        INTERFACE
        function2::function2
        tl::optional
        tl::expected
        range-v3::range-v3
        tsl::robin_map
        libassert::assert
)
