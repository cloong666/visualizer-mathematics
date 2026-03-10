#include "App.h"
#include "resources/FontData.h"
#include <string>
#include <stdexcept>

// ── Constructor ───────────────────────────────────────────────────────────────
App::App()
    : m_window(sf::VideoMode(WINDOW_W, WINDOW_H),
               "Math Visualizer – SFML",
               sf::Style::Default)
{
    m_window.setFramerateLimit(FRAME_LIMIT);
    m_window.setVerticalSyncEnabled(true);

    loadFont();

    m_functions = getBuiltinFunctions();
    loadFunction(0);
}

// ── Font loading ──────────────────────────────────────────────────────────────
void App::loadFont() {
    // 1. Try the embedded font bytes (self-contained; always works)
    if (m_font.loadFromMemory(FONT_DATA, FONT_DATA_SIZE)) {
        m_fontLoaded = true;
        return;
    }

    // 2. Fallback: try common Windows system fonts
    const char* systemFonts[] = {
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        nullptr
    };
    for (const char** p = systemFonts; *p != nullptr; ++p) {
        if (m_font.loadFromFile(*p)) {
            m_fontLoaded = true;
            return;
        }
    }
    // Continue without text – the visualizer still works, just without labels.
}

// ── Load & compile a function by index ───────────────────────────────────────
void App::loadFunction(int index) {
    if (m_functions.empty()) return;
    m_funcIndex = (index % static_cast<int>(m_functions.size())
                   + static_cast<int>(m_functions.size()))
                  % static_cast<int>(m_functions.size());

    std::string errorMsg;
    m_parser.compile(m_functions[m_funcIndex].expr, errorMsg);
    // Error is non-fatal; the renderer will simply skip invalid points.
}

// ── Main loop ─────────────────────────────────────────────────────────────────
void App::run() {
    while (m_window.isOpen()) {
        handleEvents();
        render();
    }
}

// ── Event dispatch ────────────────────────────────────────────────────────────
void App::handleEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            m_window.close();
            break;
        case sf::Event::MouseButtonPressed:
            onMouseButtonPressed(event);
            break;
        case sf::Event::MouseButtonReleased:
            onMouseButtonReleased(event);
            break;
        case sf::Event::MouseMoved:
            onMouseMoved(event);
            break;
        case sf::Event::MouseWheelScrolled:
            onMouseWheelScrolled(event);
            break;
        case sf::Event::KeyPressed:
            onKeyPressed(event);
            break;
        case sf::Event::Resized: {
            // Keep the logical view matching the new window size
            sf::FloatRect visibleArea(0, 0,
                static_cast<float>(event.size.width),
                static_cast<float>(event.size.height));
            m_window.setView(sf::View(visibleArea));
            break;
        }
        default:
            break;
        }
    }
}

// ── Input handlers ────────────────────────────────────────────────────────────
void App::onMouseButtonPressed(const sf::Event& e) {
    if (e.mouseButton.button == sf::Mouse::Left) {
        m_dragging    = true;
        m_dragStart   = sf::Vector2i(e.mouseButton.x, e.mouseButton.y);
        m_dragOffsetX = m_view.offsetX;
        m_dragOffsetY = m_view.offsetY;
    }
}

void App::onMouseButtonReleased(const sf::Event& e) {
    if (e.mouseButton.button == sf::Mouse::Left) {
        m_dragging = false;
    }
}

void App::onMouseMoved(const sf::Event& e) {
    if (!m_dragging) return;
    // Restore the saved offset then pan by the total drag delta
    m_view.offsetX = m_dragOffsetX;
    m_view.offsetY = m_dragOffsetY;
    const float dx = static_cast<float>(e.mouseMove.x - m_dragStart.x);
    const float dy = static_cast<float>(e.mouseMove.y - m_dragStart.y);
    m_view.pan(dx, dy);
}

void App::onMouseWheelScrolled(const sf::Event& e) {
    // e.mouseWheelScroll.delta > 0 → scroll up → zoom in
    const float factor = (e.mouseWheelScroll.delta > 0) ? 1.1f : (1.0f / 1.1f);
    const float H = static_cast<float>(m_window.getSize().y);
    m_view.zoom(factor,
                static_cast<float>(e.mouseWheelScroll.x),
                static_cast<float>(e.mouseWheelScroll.y),
                H);
}

void App::onKeyPressed(const sf::Event& e) {
    switch (e.key.code) {
    case sf::Keyboard::Escape:
        m_window.close();
        break;
    case sf::Keyboard::R:
        m_view.reset();
        break;
    case sf::Keyboard::Right:
    case sf::Keyboard::D:
        loadFunction(m_funcIndex + 1);
        break;
    case sf::Keyboard::Left:
    case sf::Keyboard::A:
        loadFunction(m_funcIndex - 1);
        break;
    default:
        break;
    }
}

// ── Render ────────────────────────────────────────────────────────────────────
void App::render() {
    m_renderer.draw(m_window, m_view, m_font, m_fontLoaded,
                    m_functions[m_funcIndex].name,
                    m_parser,
                    m_funcIndex,
                    static_cast<int>(m_functions.size()));
    m_window.display();
}
