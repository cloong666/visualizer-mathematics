cmake_minimum_required(VERSION 3.16)

project(MathVisualizer
    VERSION 1.1.0
    DESCRIPTION "2D Cartesian Coordinate Curve Visualizer – SFML GUI"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Options ──────────────────────────────────────────────────
option(BUILD_SFML_VISUALIZER "Build the SFML-based visualizer" ON)
option(BUILD_CPP_CORE        "Build the pure-C++ core skeleton (vm_core + vm_cli + tests)" ON)

# ============================================================
#  SFML-based interactive math visualizer
# ============================================================
if(BUILD_SFML_VISUALIZER)
    # Try to find a system-installed SFML first; fall back to FetchContent so
    # the build is fully self-contained when SFML is not installed.
    find_package(SFML 2.6 COMPONENTS graphics window system QUIET)

    if(NOT SFML_FOUND)
        message(STATUS "SFML not found locally – downloading via FetchContent (SFML 2.6.2)")
        include(FetchContent)
        FetchContent_Declare(
            SFML
            GIT_REPOSITORY https://github.com/SFML/SFML.git
            GIT_TAG        2.6.2
            GIT_SHALLOW    TRUE
        )
        # Only build the components we need
        set(SFML_BUILD_AUDIO      OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_NETWORK    OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_DOC        OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_EXAMPLES   OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(SFML)
        set(SFML_LIBRARIES sfml-graphics sfml-window sfml-system)
    else()
        message(STATUS "Using system SFML ${SFML_VERSION}")
        set(SFML_LIBRARIES sfml-graphics sfml-window sfml-system)
    endif()

    set(SFML_SOURCES
        sfml/main.cpp
        sfml/App.cpp
        sfml/Renderer.cpp
        src/ExprParser.cpp      # reuse the existing expression parser
    )

    set(SFML_HEADERS
        sfml/App.h
        sfml/Renderer.h
        sfml/ViewTransform.h
        sfml/MathFunctions.h
        sfml/resources/FontData.h
        src/ExprParser.h
    )

    add_executable(MathVisualizer ${SFML_SOURCES} ${SFML_HEADERS})

    target_link_libraries(MathVisualizer PRIVATE ${SFML_LIBRARIES})

    target_include_directories(MathVisualizer PRIVATE
        sfml
        src
    )

    if(MSVC)
        target_compile_definitions(MathVisualizer PRIVATE
            _USE_MATH_DEFINES
            _CRT_SECURE_NO_WARNINGS
        )
    endif()

    # On Windows, copy SFML DLLs next to the exe after build
    if(WIN32)
        add_custom_command(TARGET MathVisualizer POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:MathVisualizer>
                $<TARGET_FILE_DIR:MathVisualizer>
            COMMAND_EXPAND_LISTS
        )
    endif()

    install(TARGETS MathVisualizer
        RUNTIME DESTINATION bin
        BUNDLE  DESTINATION .
    )
endif()

# ============================================================
#  Pure C++ core skeleton (vm_core + vm_cli + CTest tests)
# ============================================================
if(BUILD_CPP_CORE)
    add_subdirectory(cpp)
endif()
