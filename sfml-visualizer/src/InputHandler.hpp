#pragma once

#include <SFML/Graphics.hpp>
#include "Renderer.hpp"  // for ViewState

/// Processes SFML events and mutates the application state (view, selection, params).
class InputHandler {
public:
    InputHandler();

    /// Process a single SFML event.  Returns false if the app should quit.
    bool handleEvent(const sf::Event& event,
                     sf::RenderWindow& window,
                     ViewState&        view,
                     int&              selectedIndex,
                     int               curveCount,
                     bool&             showAll,
                     float&            amplitude,
                     float&            frequency);

private:
    bool         m_dragging     = false;
    sf::Vector2f m_dragStartScreen;   ///< Mouse position when drag began (screen)
    sf::Vector2f m_dragStartOffset;   ///< View offset when drag began (world)
};
