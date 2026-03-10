#pragma once
#include <SFML/Graphics.hpp>
#include "ViewTransform.h"
#include "MathFunctions.h"
#include "Renderer.h"
#include "../src/ExprParser.h"
#include <vector>
#include <string>

// ──────────────────────────────────────────────────────────────────────────────
// App: owns the SFML window, drives the event loop, handles all user input,
// and delegates rendering to Renderer.
//
// Responsibilities:
//   • Window creation and frame-rate control
//   • Mouse drag (pan) and scroll (zoom) input
//   • Keyboard: Left/Right arrows cycle functions, R resets view, Esc quits
//   • Maintains the active ViewTransform and the selected function
// ──────────────────────────────────────────────────────────────────────────────
class App {
public:
    App();

    // Run the application; returns when the window is closed.
    void run();

private:
    // ── Initialisation ────────────────────────────────────────────────────────
    void loadFont();
    void loadFunction(int index);

    // ── Per-frame steps ───────────────────────────────────────────────────────
    void handleEvents();
    void render();

    // ── Input helpers ─────────────────────────────────────────────────────────
    void onMouseButtonPressed (const sf::Event& e);
    void onMouseButtonReleased(const sf::Event& e);
    void onMouseMoved         (const sf::Event& e);
    void onMouseWheelScrolled (const sf::Event& e);
    void onKeyPressed         (const sf::Event& e);

    // ── Window constants ──────────────────────────────────────────────────────
    static constexpr unsigned int WINDOW_W    = 1200u;
    static constexpr unsigned int WINDOW_H    =  800u;
    static constexpr unsigned int FRAME_LIMIT =   60u;

    // ── SFML objects ──────────────────────────────────────────────────────────
    sf::RenderWindow m_window;
    sf::Font         m_font;
    bool             m_fontLoaded{false};

    // ── View & math state ─────────────────────────────────────────────────────
    ViewTransform              m_view;
    Renderer                   m_renderer;
    std::vector<FunctionDef>   m_functions;
    int                        m_funcIndex{0};
    ExprParser                 m_parser;

    // ── Pan drag state ────────────────────────────────────────────────────────
    bool         m_dragging{false};
    sf::Vector2i m_dragStart;
    double       m_dragOffsetX{0.0};
    double       m_dragOffsetY{0.0};
};
