#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "ViewTransform.h"
#include "../src/ExprParser.h"

// ──────────────────────────────────────────────────────────────────────────────
// Renderer: stateless drawing utilities for the math visualizer.
//
// All draw*() methods accept an sf::RenderTarget so they work with both a
// RenderWindow and an off-screen RenderTexture.
// ──────────────────────────────────────────────────────────────────────────────
class Renderer {
public:
    // Number of horizontal sample points used to build the curve polyline.
    static constexpr int CURVE_SAMPLES = 2000;

    // Draw the full scene: grid, axes, curve, and HUD text.
    void draw(sf::RenderTarget& target,
              const ViewTransform& view,
              const sf::Font& font,
              bool fontLoaded,
              const std::string& funcName,
              ExprParser& parser,
              int funcIndex,
              int funcCount);

private:
    void drawGrid (sf::RenderTarget& target, const ViewTransform& view);
    void drawAxes (sf::RenderTarget& target, const ViewTransform& view,
                   const sf::Font& font, bool fontLoaded);
    void drawCurve(sf::RenderTarget& target, const ViewTransform& view,
                   ExprParser& parser);
    void drawHUD  (sf::RenderTarget& target, const sf::Font& font,
                   bool fontLoaded, const std::string& funcName,
                   int funcIndex, int funcCount,
                   unsigned int winWidth, unsigned int winHeight);

    // Helper: create a labelled tick text
    static sf::Text makeTick(const sf::Font& font, const std::string& str,
                             float x, float y, unsigned int charSize);
};
