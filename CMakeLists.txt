cmake_minimum_required(VERSION 3.16)

project(CurveVisualizer
    VERSION 1.0.0
    DESCRIPTION "2D/3D Cartesian Coordinate Curve Visualizer"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt auto-tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ── Find Qt (prefer Qt6, fall back to Qt5) ──────────────────
find_package(Qt6 COMPONENTS Core Widgets QUIET)
if(Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
    message(STATUS "Using Qt6")
else()
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
    set(QT_VERSION_MAJOR 5)
    message(STATUS "Using Qt5 (Qt6 not found)")
endif()

# ── Sources ─────────────────────────────────────────────────
set(SOURCES
    src/main.cpp
    src/ExprParser.cpp
    src/Plot2DWidget.cpp
    src/Plot3DWidget.cpp
    src/MainWindow.cpp
)

set(HEADERS
    src/ExprParser.h
    src/CurveData.h
    src/Plot2DWidget.h
    src/Plot3DWidget.h
    src/MainWindow.h
)

# ── Executable ──────────────────────────────────────────────
if(WIN32)
    add_executable(CurveVisualizer WIN32 ${SOURCES} ${HEADERS})
else()
    add_executable(CurveVisualizer ${SOURCES} ${HEADERS})
endif()

# ── Link Qt ─────────────────────────────────────────────────
if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(CurveVisualizer PRIVATE
        Qt6::Core
        Qt6::Widgets
    )
else()
    target_link_libraries(CurveVisualizer PRIVATE
        Qt5::Core
        Qt5::Widgets
    )
endif()

# ── Include dirs ────────────────────────────────────────────
target_include_directories(CurveVisualizer PRIVATE src)

# ── MSVC-specific: expose M_PI / M_E ─────────────────────────
if(MSVC)
    target_compile_definitions(CurveVisualizer PRIVATE _USE_MATH_DEFINES)
endif()

# ── Install (optional) ──────────────────────────────────────
install(TARGETS CurveVisualizer
    RUNTIME DESTINATION bin
    BUNDLE  DESTINATION .
)
