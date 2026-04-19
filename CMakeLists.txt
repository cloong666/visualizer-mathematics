cmake_minimum_required(VERSION 3.16)

project(CurveVisualizer
    VERSION 1.0.0
    DESCRIPTION "2D/3D Cartesian Coordinate Curve Visualizer"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Options ──────────────────────────────────────────────────
option(BUILD_QT_VISUALIZER   "Build the Qt-based visualizer"   ON)
option(BUILD_SFML_VISUALIZER "Build the SFML-based visualizer" ON)
option(BUILD_CPP_CORE        "Build the pure-C++ core skeleton (vm_core + vm_cli + tests)" ON)

# ============================================================
#  Qt-based visualizer (existing)
# ============================================================
if(BUILD_QT_VISUALIZER)
    # Qt auto-tools
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    # Find Qt (prefer Qt6, fall back to Qt5)
    find_package(Qt6 COMPONENTS Core Widgets QUIET)
    if(Qt6_FOUND)
        set(QT_VERSION_MAJOR 6)
        message(STATUS "Using Qt6")
    else()
        find_package(Qt5 COMPONENTS Core Widgets QUIET)
        if(Qt5_FOUND)
            set(QT_VERSION_MAJOR 5)
            message(STATUS "Using Qt5 (Qt6 not found)")
        else()
            message(STATUS "Qt not found – skipping Qt visualizer")
            set(BUILD_QT_VISUALIZER OFF)
        endif()
    endif()
endif()

if(BUILD_QT_VISUALIZER)
    set(QT_SOURCES
        src/main.cpp
        src/ExprParser.cpp
        src/Plot2DWidget.cpp
        src/Plot3DWidget.cpp
        src/MainWindow.cpp
    )
    set(QT_HEADERS
        src/ExprParser.h
        src/CurveData.h
        src/Plot2DWidget.h
        src/Plot3DWidget.h
        src/MainWindow.h
    )

    if(WIN32)
        add_executable(CurveVisualizer WIN32 ${QT_SOURCES} ${QT_HEADERS})
    else()
        add_executable(CurveVisualizer ${QT_SOURCES} ${QT_HEADERS})
    endif()

    if(QT_VERSION_MAJOR EQUAL 6)
        target_link_libraries(CurveVisualizer PRIVATE Qt6::Core Qt6::Widgets)
    else()
        target_link_libraries(CurveVisualizer PRIVATE Qt5::Core Qt5::Widgets)
    endif()

    target_include_directories(CurveVisualizer PRIVATE src)

    if(MSVC)
        target_compile_definitions(CurveVisualizer PRIVATE _USE_MATH_DEFINES)
    endif()

    install(TARGETS CurveVisualizer
        RUNTIME DESTINATION bin
        BUNDLE  DESTINATION .
    )
endif()

# ============================================================
#  SFML-based interactive math visualizer (new)
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
