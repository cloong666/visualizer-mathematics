#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "MathFunctions.hpp"

/// Holds the current camera/view state shared between Renderer and InputHandler.
struct ViewState {
    sf::Vector2f offset;  ///< World-space coordinates at the screen centre
    float        scale;   ///< Pixels per world unit
};

/// All SFML drawing logic: grid, axes, curves, and HUD overlay.
class Renderer {
public:
    Renderer(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded);

    // ── High-level draw calls ─────────────────────────────────────────────
    void drawScene(const ViewState& view,
                   const std::vector<MathCurve>& curves,
                   int   selectedIndex,
                   bool  showAll,
                   float amplitude,
                   float frequency);

private:
    // ── Sub-renderers ─────────────────────────────────────────────────────
    void drawGrid(const ViewState& view);
    void drawAxes(const ViewState& view);
    void drawCurve(const ViewState& view,
                   const MathCurve& curve,
                   sf::Color        color,
                   float            amplitude,
                   float            frequency);
    void drawHUD(const std::vector<MathCurve>& curves,
                 int   selectedIndex,
                 bool  showAll,
                 float amplitude,
                 float frequency,
                 const ViewState& view);

    // ── Coordinate helpers ────────────────────────────────────────────────
    sf::Vector2f worldToScreen(sf::Vector2f worldPos, const ViewState& view) const;

    // ── Member data ───────────────────────────────────────────────────────
    sf::RenderWindow& m_window;
    const sf::Font&   m_font;
    bool              m_fontLoaded;

    // Reusable vertex arrays to avoid per-frame allocations
    sf::VertexArray   m_gridLines;
    sf::VertexArray   m_axisLines;
    sf::VertexArray   m_curveStrip;
};
