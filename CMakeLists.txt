cmake_minimum_required(VERSION 3.21)

include(FetchContent)
FetchContent_Declare(
        serious-cmake-library
        GIT_REPOSITORY git@github.com:UsatiyNyan/serious-cmake-library.git
        GIT_TAG main
)
FetchContent_MakeAvailable(serious-cmake-library)

sl_project_prologue(
    C_STANDARD 17
    CXX_STANDARD 23)
project(
        serious-meta-library
        VERSION 1.0.3
        DESCRIPTION "metaprogramming library"
        HOMEPAGE_URL "TODO"
        LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
add_library(sl::meta ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE include)

add_subdirectory(dependencies)

# tie as tuple generation
include(cmake/generate_tie_as_tuple.cmake)
set(SL_GENERATE_N_FIELDS 64 CACHE STRING "Max amount of fields to generate tie_as_tuple for")
sl_generate_tie_as_tuple(${SL_GENERATE_N_FIELDS})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/include)

# test fixture
add_subdirectory(fixture)

if (NOT PROJECT_IS_TOP_LEVEL)
    return()
endif ()

sl_compiler_warnings(${PROJECT_NAME} WARNINGS_AS_ERRORS)

include(CTest)
if (BUILD_TESTING)
    add_subdirectory(test)
endif ()
