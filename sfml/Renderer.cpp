#include "Renderer.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <vector>
#include <limits>

// ── colour palette ────────────────────────────────────────────────────────────
static const sf::Color COLOR_BACKGROUND  { 15,  20,  35 };
static const sf::Color COLOR_GRID        { 40,  50,  70 };
static const sf::Color COLOR_AXIS        { 80, 100, 130 };
static const sf::Color COLOR_AXIS_MAIN   {120, 150, 200 };
static const sf::Color COLOR_CURVE       { 90, 200, 120 };
static const sf::Color COLOR_TICK_TEXT   {160, 180, 210 };
static const sf::Color COLOR_HUD_BG      {  0,   0,   0, 160 };
static const sf::Color COLOR_HUD_FUNC    {255, 220,  80 };
static const sf::Color COLOR_HUD_TEXT    {200, 215, 235 };

// ── constants ─────────────────────────────────────────────────────────────────
static constexpr unsigned int TICK_CHAR_SIZE          = 12u;
static constexpr unsigned int HUD_FUNC_SIZE           = 20u;
static constexpr unsigned int HUD_HINT_SIZE           = 13u;
static constexpr float        TICK_LENGTH             = 5.0f;
static constexpr float        CURVE_LINE_THICKNESS    = 1.8f;
// Maximum off-screen y multiplier: filters extreme asymptote spikes
static constexpr float        ASYMPTOTE_SCREEN_LIMIT  = 20.0f;

// ── numeric formatting helper ─────────────────────────────────────────────────
static std::string fmtNum(double v) {
    // Choose precision so small numbers look clean
    if (v == 0.0) return "0";
    std::ostringstream ss;
    if (std::abs(v) >= 10000 || (std::abs(v) < 0.001 && v != 0.0))
        ss << std::scientific << std::setprecision(1) << v;
    else if (std::abs(v) < 0.1)
        ss << std::fixed << std::setprecision(3) << v;
    else if (std::abs(v) < 1.0)
        ss << std::fixed << std::setprecision(2) << v;
    else if (std::fmod(v, 1.0) == 0.0)
        ss << static_cast<long long>(v);
    else
        ss << std::fixed << std::setprecision(1) << v;
    return ss.str();
}

// ── sf::Text helper ───────────────────────────────────────────────────────────
sf::Text Renderer::makeTick(const sf::Font& font, const std::string& str,
                             float x, float y, unsigned int charSize)
{
    sf::Text t;
    t.setFont(font);
    t.setString(str);
    t.setCharacterSize(charSize);
    t.setFillColor(COLOR_TICK_TEXT);
    t.setPosition(x, y);
    return t;
}

// ── drawGrid ──────────────────────────────────────────────────────────────────
void Renderer::drawGrid(sf::RenderTarget& target, const ViewTransform& view) {
    const auto  sz     = target.getSize();
    const float W      = static_cast<float>(sz.x);
    const float H      = static_cast<float>(sz.y);
    const double step  = view.gridStep();

    // Round to nearest grid line
    const double xStart = std::floor(view.screenToWorldX(0)    / step) * step;
    const double yStart = std::floor(view.screenToWorldY(H, H) / step) * step;
    const double xEnd   = view.screenToWorldX(W);
    const double yEnd   = view.screenToWorldY(0, H);

    std::vector<sf::Vertex> lines;
    lines.reserve(256);

    // Vertical grid lines
    for (double gx = xStart; gx <= xEnd + step * 0.5; gx += step) {
        float sx = view.worldToScreenX(gx);
        lines.push_back(sf::Vertex(sf::Vector2f(sx, 0), COLOR_GRID));
        lines.push_back(sf::Vertex(sf::Vector2f(sx, H), COLOR_GRID));
    }
    // Horizontal grid lines
    for (double gy = yStart; gy <= yEnd + step * 0.5; gy += step) {
        float sy = view.worldToScreenY(gy, H);
        lines.push_back(sf::Vertex(sf::Vector2f(0, sy), COLOR_GRID));
        lines.push_back(sf::Vertex(sf::Vector2f(W, sy), COLOR_GRID));
    }

    if (!lines.empty())
        target.draw(lines.data(), lines.size(), sf::Lines);
}

// ── drawAxes ──────────────────────────────────────────────────────────────────
void Renderer::drawAxes(sf::RenderTarget& target, const ViewTransform& view,
                        const sf::Font& font, bool fontLoaded)
{
    const auto  sz  = target.getSize();
    const float W   = static_cast<float>(sz.x);
    const float H   = static_cast<float>(sz.y);
    const double step = view.gridStep();

    // Axis lines (draw even if the origin is off-screen – clamp to viewport)
    float axisX = std::max(0.0f, std::min(W, view.worldToScreenX(0.0)));
    float axisY = std::max(0.0f, std::min(H, view.worldToScreenY(0.0, H)));

    // Y-axis (vertical line at x=0)
    {
        sf::VertexArray va(sf::Lines, 2);
        va[0] = sf::Vertex(sf::Vector2f(axisX, 0), COLOR_AXIS_MAIN);
        va[1] = sf::Vertex(sf::Vector2f(axisX, H), COLOR_AXIS_MAIN);
        target.draw(va);
    }
    // X-axis (horizontal line at y=0)
    {
        sf::VertexArray va(sf::Lines, 2);
        va[0] = sf::Vertex(sf::Vector2f(0, axisY), COLOR_AXIS_MAIN);
        va[1] = sf::Vertex(sf::Vector2f(W, axisY), COLOR_AXIS_MAIN);
        target.draw(va);
    }

    if (!fontLoaded) return;

    // Tick marks + labels on X axis
    const double xStart = std::floor(view.screenToWorldX(0)    / step) * step;
    const double xEnd   = view.screenToWorldX(W);
    for (double gx = xStart; gx <= xEnd + step * 0.5; gx += step) {
        if (std::abs(gx) < step * 0.01) continue;  // skip origin
        float sx = view.worldToScreenX(gx);
        // Tick line
        sf::VertexArray tick(sf::Lines, 2);
        tick[0] = sf::Vertex(sf::Vector2f(sx, axisY - TICK_LENGTH), COLOR_AXIS);
        tick[1] = sf::Vertex(sf::Vector2f(sx, axisY + TICK_LENGTH), COLOR_AXIS);
        target.draw(tick);
        // Label
        auto t = makeTick(font, fmtNum(gx), sx + 2.0f,
                          axisY + TICK_LENGTH + 1.0f, TICK_CHAR_SIZE);
        target.draw(t);
    }

    // Tick marks + labels on Y axis
    const double yStart = std::floor(view.screenToWorldY(H, H) / step) * step;
    const double yEnd   = view.screenToWorldY(0, H);
    for (double gy = yStart; gy <= yEnd + step * 0.5; gy += step) {
        if (std::abs(gy) < step * 0.01) continue;
        float sy = view.worldToScreenY(gy, H);
        sf::VertexArray tick(sf::Lines, 2);
        tick[0] = sf::Vertex(sf::Vector2f(axisX - TICK_LENGTH, sy), COLOR_AXIS);
        tick[1] = sf::Vertex(sf::Vector2f(axisX + TICK_LENGTH, sy), COLOR_AXIS);
        target.draw(tick);
        auto t = makeTick(font, fmtNum(gy), axisX + TICK_LENGTH + 2.0f,
                          sy - TICK_CHAR_SIZE * 0.6f, TICK_CHAR_SIZE);
        target.draw(t);
    }

    // Axis name labels
    sf::Text xLabel = makeTick(font, "x", W - 14.0f,
                               axisY - TICK_CHAR_SIZE * 1.8f, TICK_CHAR_SIZE + 2);
    xLabel.setFillColor(COLOR_AXIS_MAIN);
    target.draw(xLabel);

    sf::Text yLabel = makeTick(font, "y", axisX + 4.0f, 2.0f, TICK_CHAR_SIZE + 2);
    yLabel.setFillColor(COLOR_AXIS_MAIN);
    target.draw(yLabel);
}

// ── drawCurve ─────────────────────────────────────────────────────────────────
void Renderer::drawCurve(sf::RenderTarget& target, const ViewTransform& view,
                          ExprParser& parser)
{
    const auto  sz = target.getSize();
    const float W  = static_cast<float>(sz.x);
    const float H  = static_cast<float>(sz.y);

    const double xMin = view.screenToWorldX(0);
    const double xMax = view.screenToWorldX(W);
    const double dx   = (xMax - xMin) / (CURVE_SAMPLES - 1);

    // Use sf::Lines (pairs) so gaps in the curve (NaN / domain errors) are
    // represented cleanly without spurious connecting segments.
    std::vector<sf::Vertex> verts;
    verts.reserve(CURVE_SAMPLES * 2);

    // Maximum allowed screen-space y extent to filter extreme asymptotes
    const float maxScreenY = H * ASYMPTOTE_SCREEN_LIMIT;

    bool   prevValid = false;
    float  prevSx = 0.0f, prevSy = 0.0f;

    for (int i = 0; i < CURVE_SAMPLES; ++i) {
        const double x = xMin + i * dx;
        bool valid = true;
        parser.setVar("x", x);
        const double y = parser.evaluate(valid);

        if (!valid || !std::isfinite(y)) {
            prevValid = false;
            continue;
        }

        const float sx = view.worldToScreenX(x);
        const float sy = view.worldToScreenY(y, H);

        // Skip points that are astronomically far off-screen (asymptotes)
        if (std::abs(sy) > maxScreenY) {
            prevValid = false;
            continue;
        }

        if (prevValid) {
            // Emit one line segment (two vertices)
            verts.push_back(sf::Vertex(sf::Vector2f(prevSx, prevSy), COLOR_CURVE));
            verts.push_back(sf::Vertex(sf::Vector2f(sx,     sy    ), COLOR_CURVE));
        }

        prevSx    = sx;
        prevSy    = sy;
        prevValid = true;
    }

    if (!verts.empty())
        target.draw(verts.data(), verts.size(), sf::Lines);
}

// ── drawHUD ───────────────────────────────────────────────────────────────────
void Renderer::drawHUD(sf::RenderTarget& target, const sf::Font& font,
                        bool fontLoaded, const std::string& funcName,
                        int funcIndex, int funcCount,
                        unsigned int winWidth, unsigned int winHeight)
{
    if (!fontLoaded) return;

    // ── Function label (top-centre) ───────────────────────────────────────────
    {
        sf::Text funcText;
        funcText.setFont(font);
        funcText.setCharacterSize(HUD_FUNC_SIZE);
        funcText.setFillColor(COLOR_HUD_FUNC);
        const std::string label = "y = " + funcName
            + "  [" + std::to_string(funcIndex + 1) + "/" + std::to_string(funcCount) + "]";
        funcText.setString(label);
        sf::FloatRect bounds = funcText.getLocalBounds();
        funcText.setPosition(
            (winWidth  - bounds.width)  * 0.5f,
            8.0f
        );
        // Semi-transparent background
        sf::RectangleShape bg;
        bg.setSize(sf::Vector2f(bounds.width + 16.0f, bounds.height + 12.0f));
        bg.setFillColor(COLOR_HUD_BG);
        bg.setPosition(funcText.getPosition().x - 8.0f, 4.0f);
        target.draw(bg);
        target.draw(funcText);
    }

    // ── Controls hint (bottom-left) ───────────────────────────────────────────
    {
        const std::vector<std::string> hints = {
            "Drag: pan view",
            "Scroll: zoom in/out",
            "Left/Right: switch function",
            "R: reset view",
            "Esc: quit",
        };
        const float lineH = HUD_HINT_SIZE * 1.5f;
        const float startY = static_cast<float>(winHeight) - lineH * hints.size() - 8.0f;

        // Background panel
        sf::RectangleShape panel;
        panel.setSize(sf::Vector2f(200.0f, lineH * hints.size() + 8.0f));
        panel.setFillColor(COLOR_HUD_BG);
        panel.setPosition(6.0f, startY - 4.0f);
        target.draw(panel);

        for (std::size_t i = 0; i < hints.size(); ++i) {
            sf::Text t;
            t.setFont(font);
            t.setCharacterSize(HUD_HINT_SIZE);
            t.setFillColor(COLOR_HUD_TEXT);
            t.setString(hints[i]);
            t.setPosition(10.0f, startY + static_cast<float>(i) * lineH);
            target.draw(t);
        }
    }
}

// ── Public draw entry-point ───────────────────────────────────────────────────
void Renderer::draw(sf::RenderTarget& target,
                    const ViewTransform& view,
                    const sf::Font& font,
                    bool fontLoaded,
                    const std::string& funcName,
                    ExprParser& parser,
                    int funcIndex,
                    int funcCount)
{
    const auto sz = target.getSize();

    target.clear(COLOR_BACKGROUND);
    drawGrid(target, view);
    drawAxes(target, view, font, fontLoaded);
    drawCurve(target, view, parser);
    drawHUD(target, font, fontLoaded, funcName,
            funcIndex, funcCount, sz.x, sz.y);
}
