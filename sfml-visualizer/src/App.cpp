#include "App.hpp"
#include "Constants.hpp"
#include <vector>
#include <string>

// Ordered list of font paths to try at startup
static const std::vector<std::string> FONT_SEARCH_PATHS = {
    "assets/tuffy.ttf",
    "assets/sansation.ttf",
    "assets/font.ttf",
    // Windows system fonts (always present on Windows CI runners and end-user PCs)
    "C:/Windows/Fonts/Arial.ttf",
    "C:/Windows/Fonts/Calibri.ttf",
    "C:/Windows/Fonts/Consolas.ttf",
    "C:/Windows/Fonts/Verdana.ttf",
    // Linux / CI fallbacks
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/TTF/DejaVuSans.ttf",
    "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
};

// ── Constructor ───────────────────────────────────────────────────────────────
App::App()
    : m_window(sf::VideoMode(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT),
               Constants::WINDOW_TITLE,
               sf::Style::Default)
{
    m_window.setFramerateLimit(Constants::FRAME_RATE);

    // Initialise view to centre on origin
    m_view.offset = {0.f, 0.f};
    m_view.scale  = Constants::INITIAL_SCALE;
    m_amplitude   = Constants::DEFAULT_AMPLITUDE;
    m_frequency   = Constants::DEFAULT_FREQUENCY;

    m_fontLoaded = loadFont();

    m_renderer = std::make_unique<Renderer>(m_window, m_font, m_fontLoaded);
    m_input    = std::make_unique<InputHandler>();
}

// ── Font loader ───────────────────────────────────────────────────────────────
bool App::loadFont() {
    for (const auto& path : FONT_SEARCH_PATHS) {
        if (m_font.loadFromFile(path)) {
            return true;
        }
    }
    return false;
}

// ── Main loop ─────────────────────────────────────────────────────────────────
void App::run() {
    const auto& curves = getCurves();

    while (m_window.isOpen()) {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            bool running = m_input->handleEvent(
                event, m_window,
                m_view,
                m_selectedIndex,
                static_cast<int>(curves.size()),
                m_showAll,
                m_amplitude,
                m_frequency
            );
            if (!running) return;
        }

        m_renderer->drawScene(
            m_view, curves,
            m_selectedIndex,
            m_showAll,
            m_amplitude,
            m_frequency
        );
    }
}
