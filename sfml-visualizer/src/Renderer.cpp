#include "Renderer.hpp"
#include "Constants.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <limits>
#include <array>

// ── Palette of distinct curve colours ────────────────────────────────────────
static const std::array<sf::Color, 10> CURVE_COLORS = {{
    sf::Color(64,  196, 255),   // cyan-blue  (selected / first)
    sf::Color(255, 140,  64),   // orange
    sf::Color(100, 230, 100),   // green
    sf::Color(255, 210,  60),   // yellow
    sf::Color(210,  80, 255),   // violet
    sf::Color(255,  80, 140),   // pink
    sf::Color( 60, 220, 190),   // teal
    sf::Color(200, 200, 255),   // lavender
    sf::Color(255, 160, 100),   // peach
    sf::Color(140, 255, 140),   // lime
}};

// ── Constructor ───────────────────────────────────────────────────────────────
Renderer::Renderer(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded)
    : m_window(window)
    , m_font(font)
    , m_fontLoaded(fontLoaded)
    , m_gridLines(sf::Lines)
    , m_axisLines(sf::Lines)
    , m_curveStrip(sf::LineStrip)
{}

// ── Coordinate transform ──────────────────────────────────────────────────────
sf::Vector2f Renderer::worldToScreen(sf::Vector2f worldPos, const ViewState& view) const {
    const float cx = static_cast<float>(m_window.getSize().x) / 2.0f;
    const float cy = static_cast<float>(m_window.getSize().y) / 2.0f;
    return {
        cx + (worldPos.x - view.offset.x) * view.scale,
        cy - (worldPos.y - view.offset.y) * view.scale   // flip Y
    };
}

// ── Grid ──────────────────────────────────────────────────────────────────────
void Renderer::drawGrid(const ViewState& view) {
    m_gridLines.clear();

    const float W = static_cast<float>(m_window.getSize().x);
    const float H = static_cast<float>(m_window.getSize().y);
    const float cx = W / 2.0f;
    const float cy = H / 2.0f;

    // Choose a grid spacing that's "round" in world coordinates
    // We want roughly 5-12 grid lines on screen
    float rawStep = 80.0f / view.scale; // world units per ~80 px
    float exp10    = std::pow(10.0f, std::floor(std::log10(rawStep)));
    float step;
    if      (rawStep / exp10 < 2.0f)  step = exp10;
    else if (rawStep / exp10 < 5.0f)  step = 2.0f * exp10;
    else                               step = 5.0f * exp10;

    // World range visible on screen
    float worldLeft   =  view.offset.x - cx / view.scale;
    float worldRight  =  view.offset.x + cx / view.scale;
    float worldBottom =  view.offset.y - cy / view.scale;
    float worldTop    =  view.offset.y + cy / view.scale;

    // Minor grid
    auto addGridLine = [&](sf::Vector2f a, sf::Vector2f b, uint8_t alpha) {
        sf::Color c(60, 60, 90, alpha);
        m_gridLines.append(sf::Vertex(a, c));
        m_gridLines.append(sf::Vertex(b, c));
    };

    // Vertical lines
    float xStart = std::floor(worldLeft  / step) * step;
    for (float wx = xStart; wx <= worldRight; wx += step) {
        float sx = cx + (wx - view.offset.x) * view.scale;
        bool  isMajor = (std::abs(std::fmod(wx, step * 5.0f)) < step * 0.01f);
        uint8_t alpha = isMajor ? static_cast<uint8_t>(Constants::GRID_MAJOR_ALPHA)
                                : static_cast<uint8_t>(Constants::GRID_MINOR_ALPHA);
        addGridLine({sx, 0.f}, {sx, H}, alpha);
    }
    // Horizontal lines
    float yStart = std::floor(worldBottom / step) * step;
    for (float wy = yStart; wy <= worldTop; wy += step) {
        float sy = cy - (wy - view.offset.y) * view.scale;
        bool  isMajor = (std::abs(std::fmod(wy, step * 5.0f)) < step * 0.01f);
        uint8_t alpha = isMajor ? static_cast<uint8_t>(Constants::GRID_MAJOR_ALPHA)
                                : static_cast<uint8_t>(Constants::GRID_MINOR_ALPHA);
        addGridLine({0.f, sy}, {W, sy}, alpha);
    }

    m_window.draw(m_gridLines);

    // Axis tick labels (if font available)
    if (m_fontLoaded && step * view.scale > 30.0f) {
        // X-axis ticks
        for (float wx = xStart; wx <= worldRight; wx += step) {
            if (std::abs(wx) < step * 0.01f) continue; // skip origin
            float sx = cx + (wx - view.offset.x) * view.scale;
            float sy = cy - (0.0f  - view.offset.y) * view.scale;
            sy = std::max(4.0f, std::min(H - 20.0f, sy)); // clamp to window

            std::ostringstream oss;
            oss << std::setprecision(4) << std::noshowpoint << wx;
            sf::Text lbl(oss.str(), m_font, Constants::FONT_SIZE_SMALL - 2);
            lbl.setFillColor(sf::Color(150, 150, 170));
            lbl.setPosition(sx - lbl.getLocalBounds().width / 2.0f, sy + 3.0f);
            m_window.draw(lbl);
        }
        // Y-axis ticks
        for (float wy = yStart; wy <= worldTop; wy += step) {
            if (std::abs(wy) < step * 0.01f) continue;
            float sx = cx + (0.0f  - view.offset.x) * view.scale;
            float sy = cy - (wy   - view.offset.y) * view.scale;
            sx = std::max(2.0f, std::min(W - 40.0f, sx));

            std::ostringstream oss;
            oss << std::setprecision(4) << std::noshowpoint << wy;
            sf::Text lbl(oss.str(), m_font, Constants::FONT_SIZE_SMALL - 2);
            lbl.setFillColor(sf::Color(150, 150, 170));
            lbl.setPosition(sx + 4.0f, sy - lbl.getLocalBounds().height - 1.0f);
            m_window.draw(lbl);
        }
    }
}

// ── Axes ─────────────────────────────────────────────────────────────────────
void Renderer::drawAxes(const ViewState& view) {
    m_axisLines.clear();

    const float W  = static_cast<float>(m_window.getSize().x);
    const float H  = static_cast<float>(m_window.getSize().y);
    const sf::Color axisColor(200, 200, 210, 230);

    // X axis
    float yAxis = H / 2.0f - (0.0f - view.offset.y) * view.scale;
    yAxis = std::max(1.0f, std::min(H - 1.0f, yAxis));
    m_axisLines.append(sf::Vertex({0.f,   yAxis}, axisColor));
    m_axisLines.append(sf::Vertex({W,     yAxis}, axisColor));

    // Y axis
    float xAxis = W / 2.0f + (0.0f - view.offset.x) * view.scale;
    xAxis = std::max(1.0f, std::min(W - 1.0f, xAxis));
    m_axisLines.append(sf::Vertex({xAxis, 0.f},  axisColor));
    m_axisLines.append(sf::Vertex({xAxis, H},    axisColor));

    m_window.draw(m_axisLines);

    // Axis labels
    if (m_fontLoaded) {
        sf::Text xLabel("x", m_font, Constants::FONT_SIZE_SMALL);
        xLabel.setFillColor(axisColor);
        xLabel.setPosition(W - 18.f, yAxis - 20.f);
        m_window.draw(xLabel);

        sf::Text yLabel("y", m_font, Constants::FONT_SIZE_SMALL);
        yLabel.setFillColor(axisColor);
        yLabel.setPosition(xAxis + 5.f, 4.f);
        m_window.draw(yLabel);
    }
}

// ── Single curve ──────────────────────────────────────────────────────────────
void Renderer::drawCurve(const ViewState& view,
                          const MathCurve& curve,
                          sf::Color        color,
                          float            amplitude,
                          float            frequency)
{
    m_curveStrip.clear();
    m_curveStrip.setPrimitiveType(sf::LineStrip);

    const float W  = static_cast<float>(m_window.getSize().x);
    const float cx = W / 2.0f;

    // X range: what is visible on screen with a small margin
    float worldLeft  = view.offset.x - cx / view.scale - 1.0f;
    float worldRight = view.offset.x + cx / view.scale + 1.0f;

    const float dx = (worldRight - worldLeft) / static_cast<float>(Constants::CURVE_SAMPLES);

    double prevY = std::numeric_limits<double>::quiet_NaN();

    for (int i = 0; i <= Constants::CURVE_SAMPLES; ++i) {
        double wx = static_cast<double>(worldLeft) + i * static_cast<double>(dx);
        double wy = static_cast<double>(amplitude) * curve.fn(static_cast<double>(frequency) * wx);

        if (std::isnan(wy) || std::isinf(wy)) {
            // Gap: flush existing strip and start new one
            if (m_curveStrip.getVertexCount() >= 2) {
                m_window.draw(m_curveStrip);
            }
            m_curveStrip.clear();
            m_curveStrip.setPrimitiveType(sf::LineStrip);
            prevY = std::numeric_limits<double>::quiet_NaN();
            continue;
        }

        // Detect discontinuity (large jump) and break the strip
        if (!std::isnan(prevY) && std::abs(wy - prevY) > Constants::DISCONTINUITY_THR) {
            if (m_curveStrip.getVertexCount() >= 2) {
                m_window.draw(m_curveStrip);
            }
            m_curveStrip.clear();
            m_curveStrip.setPrimitiveType(sf::LineStrip);
        }
        prevY = wy;

        sf::Vector2f screenPos = worldToScreen({static_cast<float>(wx),
                                                 static_cast<float>(wy)}, view);
        m_curveStrip.append(sf::Vertex(screenPos, color));
    }

    if (m_curveStrip.getVertexCount() >= 2) {
        m_window.draw(m_curveStrip);
    }
}

// ── HUD ───────────────────────────────────────────────────────────────────────
void Renderer::drawHUD(const std::vector<MathCurve>& curves,
                        int   selectedIndex,
                        bool  showAll,
                        float amplitude,
                        float frequency,
                        const ViewState& view)
{
    if (!m_fontLoaded) return;

    const float M = Constants::HUD_MARGIN;
    const float P = Constants::HUD_PADDING;
    const float LH = Constants::HUD_LINE_H;
    const float W  = static_cast<float>(m_window.getSize().x);
    const float H  = static_cast<float>(m_window.getSize().y);

    // ── Info panel (top-left) ─────────────────────────────────────────────
    const std::string& selName    = curves[selectedIndex].name;
    const std::string& selFormula = curves[selectedIndex].formula;

    std::ostringstream ampStr, freqStr, scaleStr;
    ampStr   << std::fixed << std::setprecision(2) << "Amplitude: " << amplitude;
    freqStr  << std::fixed << std::setprecision(2) << "Frequency: " << frequency;
    scaleStr << std::fixed << std::setprecision(1) << "Scale: "     << view.scale << " px/unit";

    std::vector<std::string> infoLines = {
        selFormula,
        ampStr.str(),
        freqStr.str(),
        scaleStr.str(),
    };
    if (showAll) infoLines.insert(infoLines.begin(), "[Showing all curves]");

    float panelW = 300.f;
    float panelH = P * 2.f + LH * static_cast<float>(infoLines.size());

    // Semi-transparent background
    sf::RectangleShape infoBg({panelW, panelH});
    infoBg.setFillColor(sf::Color(10, 10, 20, 190));
    infoBg.setOutlineColor(sf::Color(80, 80, 120, 180));
    infoBg.setOutlineThickness(1.f);
    infoBg.setPosition(M, M);
    m_window.draw(infoBg);

    // Title line (function name large)
    sf::Text title(selName, m_font, Constants::FONT_SIZE_LARGE);
    title.setFillColor(sf::Color(100, 210, 255));
    title.setStyle(sf::Text::Bold);
    title.setPosition(M + P, M + P - 2.f);
    m_window.draw(title);

    // Detail lines below
    for (size_t i = 0; i < infoLines.size(); ++i) {
        sf::Text line(infoLines[i], m_font, Constants::FONT_SIZE_SMALL);
        line.setFillColor(sf::Color(200, 200, 220));
        line.setPosition(M + P, M + P + LH * (static_cast<float>(i) + 1.f) - 2.f);
        m_window.draw(line);
    }

    // ── Controls hint (bottom-left) ───────────────────────────────────────
    const std::vector<std::string> hints = {
        "\u2190/\u2192  Prev/Next curve",
        "\u2191/\u2193  Amplitude \u00b10.1",
        "+/-       Frequency \u00b10.1",
        "Space     Toggle all curves",
        "R         Reset view",
        "Drag      Pan view",
        "Scroll    Zoom in/out",
        "Esc       Quit",
    };

    float hintW = 260.f;
    float hintH = P * 2.f + LH * static_cast<float>(hints.size());
    sf::RectangleShape hintBg({hintW, hintH});
    hintBg.setFillColor(sf::Color(10, 10, 20, 180));
    hintBg.setOutlineColor(sf::Color(80, 80, 120, 150));
    hintBg.setOutlineThickness(1.f);
    hintBg.setPosition(M, H - M - hintH);
    m_window.draw(hintBg);

    for (size_t i = 0; i < hints.size(); ++i) {
        sf::Text hintLine(hints[i], m_font, Constants::FONT_SIZE_SMALL);
        hintLine.setFillColor(sf::Color(160, 160, 180));
        hintLine.setPosition(M + P, H - M - hintH + P + LH * static_cast<float>(i));
        m_window.draw(hintLine);
    }

    // ── Curve list (top-right) when showAll ───────────────────────────────
    if (showAll) {
        float listX = W - 220.f - M;
        float listH = P * 2.f + LH * static_cast<float>(curves.size());
        sf::RectangleShape listBg({220.f, listH});
        listBg.setFillColor(sf::Color(10, 10, 20, 180));
        listBg.setOutlineColor(sf::Color(80, 80, 120, 150));
        listBg.setOutlineThickness(1.f);
        listBg.setPosition(listX, M);
        m_window.draw(listBg);

        for (size_t i = 0; i < curves.size(); ++i) {
            sf::Color c = CURVE_COLORS[i % CURVE_COLORS.size()];
            bool selected = (static_cast<int>(i) == selectedIndex);

            // Colour swatch
            sf::RectangleShape swatch({12.f, 12.f});
            swatch.setFillColor(c);
            swatch.setPosition(listX + P, M + P + LH * static_cast<float>(i) + 3.f);
            m_window.draw(swatch);

            sf::Text lbl(curves[i].name, m_font, Constants::FONT_SIZE_SMALL);
            lbl.setFillColor(selected ? sf::Color(255, 255, 120) : c);
            if (selected) lbl.setStyle(sf::Text::Bold);
            lbl.setPosition(listX + P + 16.f, M + P + LH * static_cast<float>(i));
            m_window.draw(lbl);
        }
    }

    // ── Origin label ──────────────────────────────────────────────────────
    sf::Vector2f originScreen = worldToScreen({0.f, 0.f}, view);
    if (originScreen.x > 10.f && originScreen.x < W - 10.f &&
        originScreen.y > 10.f && originScreen.y < H - 10.f)
    {
        sf::Text origin("O", m_font, Constants::FONT_SIZE_SMALL - 1);
        origin.setFillColor(sf::Color(160, 160, 170, 200));
        origin.setPosition(originScreen.x + 3.f, originScreen.y + 2.f);
        m_window.draw(origin);
    }
}

// ── Public drawScene ──────────────────────────────────────────────────────────
void Renderer::drawScene(const ViewState&              view,
                          const std::vector<MathCurve>& curves,
                          int                           selectedIndex,
                          bool                          showAll,
                          float                         amplitude,
                          float                         frequency)
{
    m_window.clear(sf::Color(20, 20, 30));
    drawGrid(view);
    drawAxes(view);

    if (showAll) {
        for (size_t i = 0; i < curves.size(); ++i) {
            sf::Color c = CURVE_COLORS[i % CURVE_COLORS.size()];
            // Dim non-selected curves slightly
            if (static_cast<int>(i) != selectedIndex) {
                c.a = 160;
            }
            drawCurve(view, curves[i], c, amplitude, frequency);
        }
    } else {
        sf::Color c = CURVE_COLORS[selectedIndex % CURVE_COLORS.size()];
        drawCurve(view, curves[selectedIndex], c, amplitude, frequency);
    }

    drawHUD(curves, selectedIndex, showAll, amplitude, frequency, view);
    m_window.display();
}
