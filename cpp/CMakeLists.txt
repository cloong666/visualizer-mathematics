cmake_minimum_required(VERSION 3.16)

project(vm_core_skeleton
    VERSION 0.1.0
    DESCRIPTION "Mathematics Visualizer – pure C++ core skeleton"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ─────────────────────────────────────────────────────────────────────────────
# When this file is used as a standalone build (cmake -S cpp -B build_cpp),
# the ExprParser lives one level up in ../src.  When included via
# add_subdirectory() from the top-level CMakeLists.txt, CMAKE_SOURCE_DIR
# already points to the repo root.
# ─────────────────────────────────────────────────────────────────────────────
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Standalone build: adjust path to the shared expression parser.
    set(EXPR_PARSER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")
else()
    # Included from repo root.
    set(EXPR_PARSER_DIR "${CMAKE_SOURCE_DIR}/src")
endif()

# ─────────────────────────────────────────────────────────────────────────────
# vm_core – static library
# ─────────────────────────────────────────────────────────────────────────────
add_library(vm_core STATIC
    core/src/Sampler.cpp
    ${EXPR_PARSER_DIR}/ExprParser.cpp
)

target_include_directories(vm_core
    PUBLIC  core/include
    PRIVATE ${EXPR_PARSER_DIR}
)

target_compile_options(vm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

if(MSVC)
    target_compile_definitions(vm_core PRIVATE _USE_MATH_DEFINES)
endif()

# ─────────────────────────────────────────────────────────────────────────────
# vm_cli – command-line demo executable
# ─────────────────────────────────────────────────────────────────────────────
add_executable(vm_cli cli/main.cpp)

target_link_libraries(vm_cli PRIVATE vm_core)

target_compile_options(vm_cli PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# ─────────────────────────────────────────────────────────────────────────────
# Tests (CTest)
# ─────────────────────────────────────────────────────────────────────────────
enable_testing()

add_executable(test_vm_core tests/test_sampler.cpp)
target_link_libraries(test_vm_core PRIVATE vm_core)

add_test(NAME vm_core_tests COMMAND test_vm_core)
