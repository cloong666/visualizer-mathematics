#pragma once
#include <cmath>

// ──────────────────────────────────────────────────────────────────────────────
// ViewTransform: maps between mathematical world coordinates and SFML screen
// coordinates.
//
// Convention:
//   World  : x increases right, y increases up  (standard Cartesian)
//   Screen : x increases right, y increases down (SFML native)
//
// The bottom-left corner of the viewport sits at world (offsetX, offsetY).
// scale = pixels per world unit.
// ──────────────────────────────────────────────────────────────────────────────
struct ViewTransform {
    double offsetX = -10.0;  // world x of the left edge
    double offsetY = -7.0;   // world y of the bottom edge
    double scale   = 60.0;   // pixels per world unit

    // ── World → Screen ────────────────────────────────────────────────────────
    float worldToScreenX(double wx) const {
        return static_cast<float>((wx - offsetX) * scale);
    }

    float worldToScreenY(double wy, float screenHeight) const {
        return static_cast<float>(screenHeight - (wy - offsetY) * scale);
    }

    // ── Screen → World ────────────────────────────────────────────────────────
    double screenToWorldX(float sx) const {
        return sx / scale + offsetX;
    }

    double screenToWorldY(float sy, float screenHeight) const {
        return (screenHeight - sy) / scale + offsetY;
    }

    // ── Pan (pixel delta) ─────────────────────────────────────────────────────
    void pan(float dx, float dy) {
        offsetX -= dx / scale;
        offsetY += dy / scale;  // screen y is inverted relative to world y
    }

    // ── Zoom about a screen-space pivot point ─────────────────────────────────
    void zoom(float factor, float centerX, float centerY, float screenHeight) {
        double wx = screenToWorldX(centerX);
        double wy = screenToWorldY(centerY, screenHeight);
        scale *= factor;
        // Repin the pivot so the world point under the cursor doesn't move
        offsetX = wx - centerX / scale;
        offsetY = wy - (screenHeight - centerY) / scale;
    }

    // ── Reset to default view ─────────────────────────────────────────────────
    void reset() {
        offsetX = -10.0;
        offsetY = -7.0;
        scale   = 60.0;
    }

    // ── Adaptive grid step (targets ~80–100 px between grid lines) ────────────
    double gridStep() const {
        const double rawStep = 100.0 / scale;
        const double power   = std::pow(10.0, std::floor(std::log10(rawStep)));
        const double norm    = rawStep / power;
        if (norm < 1.5) return power;
        if (norm < 3.5) return 2.0 * power;
        if (norm < 7.5) return 5.0 * power;
        return 10.0 * power;
    }
};
