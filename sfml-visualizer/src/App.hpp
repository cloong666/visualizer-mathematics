#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "Renderer.hpp"
#include "InputHandler.hpp"
#include "MathFunctions.hpp"

/// Top-level application: owns the window, font, and all subsystems.
class App {
public:
    App();

    /// Run the application event/render loop. Blocks until window is closed.
    void run();

private:
    bool loadFont();

    sf::RenderWindow          m_window;
    sf::Font                  m_font;
    bool                      m_fontLoaded = false;

    // View state
    ViewState                 m_view;

    // Curve state
    int                       m_selectedIndex = 0;
    bool                      m_showAll       = false;
    float                     m_amplitude     = 1.0f;
    float                     m_frequency     = 1.0f;

    std::unique_ptr<Renderer>     m_renderer;
    std::unique_ptr<InputHandler> m_input;
};
