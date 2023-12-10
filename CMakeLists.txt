cmake_minimum_required(VERSION 3.21)

include(FetchContent)
FetchContent_Declare(
        serious-cmake-library
        GIT_REPOSITORY git@github.com:UsatiyNyan/serious-cmake-library.git
        GIT_TAG main
)
FetchContent_MakeAvailable(serious-cmake-library)

sl_project_prologue(CXX_STANDARD 23)
project(
        serious-meta-library
        VERSION 0.0.1
        DESCRIPTION "metaprogramming library"
        HOMEPAGE_URL "TODO"
        LANGUAGES CXX)

set(SL_GENERATE_N_FIELDS_IN_META_APPLY
        64 CACHE STRING
        "Specify how much fields to generate in structured-binding in meta::apply(struct)")

include(cmake/generate_meta_apply.cmake)
sl_generate_meta_apply(${SL_GENERATE_N_FIELDS_IN_META_APPLY})

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
        include
        # for generated files
        ${CMAKE_CURRENT_BINARY_DIR}/include)

if (NOT PROJECT_IS_TOP_LEVEL)
    return()
endif ()

include(CTest)
if (BUILD_TESTING)
    add_subdirectory(test)
endif ()
