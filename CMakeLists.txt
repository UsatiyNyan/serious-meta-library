cmake_minimum_required(VERSION 3.21)

include(FetchContent)
FetchContent_Declare(
        serious-cmake-library
        GIT_REPOSITORY https://github.com/UsatiyNyan/serious-cmake-library.git
        GIT_TAG main
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(serious-cmake-library)

sl_project_prologue(
    C_STANDARD 17
    CXX_STANDARD 23)
project(
        serious-meta-library
        VERSION 1.2.3
        DESCRIPTION "metaprogramming library"
        HOMEPAGE_URL "TODO"
        LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
add_library(sl::meta ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE include)
add_subdirectory(dependencies)

# setup detected
sl_detect_machine_cpu(SL_DETECTED_MACHINE_CPU)
sl_detect_compiler(SL_DETECTED_COMPILER)
sl_detect_os(SL_DETECTED_OS)

set(SL_DETECTED_MACHINE_CPU "${SL_DETECTED_MACHINE_CPU}" CACHE INTERNAL "" FORCE)
set(SL_DETECTED_COMPILER "${SL_DETECTED_COMPILER}" CACHE INTERNAL "" FORCE)
set(SL_DETECTED_OS "${SL_DETECTED_OS}" CACHE INTERNAL "" FORCE)

target_compile_definitions(${PROJECT_NAME}
   INTERFACE
   SL_CPU=${SL_DETECTED_MACHINE_CPU}
   "SL_CPU_IS_${SL_DETECTED_MACHINE_CPU}"
   SL_COMPILER=${SL_DETECTED_COMPILER}
   "SL_COMPILER_IS_${SL_DETECTED_COMPILER}"
   SL_OS=${SL_DETECTED_OS}
   "SL_OS_IS_${SL_DETECTED_OS}"
)

# tie as tuple generation
set(SL_GENERATE_N_FIELDS 64 CACHE STRING "Max amount of fields to generate tie_as_tuple for")
include(cmake/generate_tie_as_tuple.cmake)
sl_generate_tie_as_tuple(${SL_GENERATE_N_FIELDS})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/include)

# sanitizers
set(SL_SANITIZER "" CACHE STRING "Sanitizer to turn on, options are: address, thread")
if ("${SL_SANITIZER}" STREQUAL "")
elseif("${SL_SANITIZER}" STREQUAL "address")
    message(STATUS "${PROJECT_NAME} address sanitizer enabled")
    target_compile_options(${PROJECT_NAME} INTERFACE -fsanitize=address,undefined -fno-sanitize-recover=all)
    target_link_options(${PROJECT_NAME} INTERFACE -fsanitize=address,undefined)
elseif("${SL_SANITIZER}" STREQUAL "thread")
    message(STATUS "${PROJECT_NAME} thread sanitizer enabled")
    target_compile_options(${PROJECT_NAME} INTERFACE -fsanitize=thread -fno-sanitize-recover=all)
    target_link_options(${PROJECT_NAME} INTERFACE -fsanitize=thread)
else()
    message(FATAL_ERROR "incorrect SL_SANITIZER option: ${SL_SANITIZER}")
endif()

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
