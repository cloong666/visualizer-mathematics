cmake_minimum_required(VERSION 3.16)
project(MathVisualizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── SFML: build statically so the exe is self-contained (no extra DLLs) ───────
set(BUILD_SHARED_LIBS OFF)
set(SFML_BUILD_AUDIO   OFF)
set(SFML_BUILD_NETWORK OFF)

include(FetchContent)

FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(sfml)

# ── Font: copy from the fetched SFML source tree ──────────────────────────────
set(ASSETS_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
file(MAKE_DIRECTORY "${ASSETS_SRC_DIR}")

file(GLOB_RECURSE _SFML_FONTS "${sfml_SOURCE_DIR}/*.ttf")
if(_SFML_FONTS)
    list(GET _SFML_FONTS 0 _FIRST_FONT)
    get_filename_component(_FONT_NAME "${_FIRST_FONT}" NAME)
    configure_file("${_FIRST_FONT}" "${ASSETS_SRC_DIR}/${_FONT_NAME}" COPYONLY)
    message(STATUS "MathVisualizer: using bundled SFML font '${_FONT_NAME}'")
else()
    message(STATUS "MathVisualizer: no bundled font found; will fall back to system fonts")
endif()

# ── Executable ────────────────────────────────────────────────────────────────
add_executable(MathVisualizer
    src/main.cpp
    src/App.cpp
    src/Renderer.cpp
    src/InputHandler.cpp
    src/MathFunctions.cpp
)

target_include_directories(MathVisualizer PRIVATE src)

target_link_libraries(MathVisualizer PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

# ── Windows: hide console, provide WinMain entry, set icon ───────────────────
if(WIN32)
    # sfml-main provides the WinMain that calls user's main() when WIN32_EXECUTABLE is set
    target_link_libraries(MathVisualizer PRIVATE sfml-main)
    set_target_properties(MathVisualizer PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# ── Copy assets next to the exe after each build ─────────────────────────────
add_custom_command(TARGET MathVisualizer POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:MathVisualizer>/assets"
    COMMENT "Copying assets to output directory"
)
