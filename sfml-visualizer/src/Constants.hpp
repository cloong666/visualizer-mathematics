#pragma once

namespace Constants {

    // ── Window ──────────────────────────────────────────────────────────────
    constexpr unsigned int WINDOW_WIDTH  = 1280;
    constexpr unsigned int WINDOW_HEIGHT = 800;
    constexpr unsigned int FRAME_RATE    = 60;
    constexpr const char*  WINDOW_TITLE  = "Math Curve Visualizer (SFML)";

    // ── View / Camera ────────────────────────────────────────────────────────
    constexpr float INITIAL_SCALE = 80.0f;   // pixels per world unit at startup
    constexpr float ZOOM_FACTOR   = 1.12f;   // multiplier per scroll tick
    constexpr float MIN_SCALE     = 5.0f;
    constexpr float MAX_SCALE     = 4000.0f;

    // ── Curve rendering ──────────────────────────────────────────────────────
    constexpr int   CURVE_SAMPLES     = 3000; // sample points per curve draw
    constexpr float DISCONTINUITY_THR = 50.0f;// world-unit jump treated as gap

    // ── Grid ─────────────────────────────────────────────────────────────────
    constexpr float GRID_MINOR_ALPHA = 40.0f;  // 0-255
    constexpr float GRID_MAJOR_ALPHA = 100.0f;

    // ── Typography ───────────────────────────────────────────────────────────
    constexpr unsigned int FONT_SIZE_LARGE = 18;
    constexpr unsigned int FONT_SIZE_SMALL = 13;

    // ── Default parameter values ─────────────────────────────────────────────
    constexpr float DEFAULT_AMPLITUDE = 1.0f;
    constexpr float DEFAULT_FREQUENCY = 1.0f;
    constexpr float PARAM_STEP        = 0.1f;  // step for Up/Down adjustment

    // ── HUD layout constants ─────────────────────────────────────────────────
    constexpr float HUD_MARGIN  = 12.0f;
    constexpr float HUD_PADDING = 8.0f;
    constexpr float HUD_LINE_H  = 22.0f;

} // namespace Constants
