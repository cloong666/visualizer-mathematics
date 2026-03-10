#include "InputHandler.hpp"
#include "Constants.hpp"
#include <cmath>

InputHandler::InputHandler() = default;

bool InputHandler::handleEvent(const sf::Event& event,
                                sf::RenderWindow& window,
                                ViewState&        view,
                                int&              selectedIndex,
                                int               curveCount,
                                bool&             showAll,
                                float&            amplitude,
                                float&            frequency)
{
    const float W = static_cast<float>(window.getSize().x);
    const float H = static_cast<float>(window.getSize().y);

    switch (event.type) {

    // ── Window close ─────────────────────────────────────────────────────
    case sf::Event::Closed:
        window.close();
        return false;

    case sf::Event::KeyPressed:
        switch (event.key.code) {

        // Quit
        case sf::Keyboard::Escape:
            window.close();
            return false;

        // Cycle curves
        case sf::Keyboard::Right:
        case sf::Keyboard::D:
            selectedIndex = (selectedIndex + 1) % curveCount;
            break;
        case sf::Keyboard::Left:
        case sf::Keyboard::A:
            selectedIndex = (selectedIndex - 1 + curveCount) % curveCount;
            break;

        // Amplitude
        case sf::Keyboard::Up:
            amplitude = std::min(amplitude + Constants::PARAM_STEP, 10.0f);
            break;
        case sf::Keyboard::Down:
            amplitude = std::max(amplitude - Constants::PARAM_STEP, -10.0f);
            break;

        // Frequency  (= / + key and - key)
        case sf::Keyboard::Equal:   // = / + (unshifted)
        case sf::Keyboard::Add:     // numpad +
            frequency = std::min(frequency + Constants::PARAM_STEP, 10.0f);
            break;
        case sf::Keyboard::Dash:    // -
        case sf::Keyboard::Subtract:// numpad -
            frequency = std::max(frequency - Constants::PARAM_STEP, 0.1f);
            break;

        // Toggle show-all
        case sf::Keyboard::Space:
            showAll = !showAll;
            break;

        // Reset view
        case sf::Keyboard::R:
            view.offset    = {0.f, 0.f};
            view.scale     = Constants::INITIAL_SCALE;
            amplitude      = Constants::DEFAULT_AMPLITUDE;
            frequency      = Constants::DEFAULT_FREQUENCY;
            break;

        default:
            break;
        }
        break;

    // ── Mouse drag (pan) ──────────────────────────────────────────────────
    case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button == sf::Mouse::Left) {
            m_dragging          = true;
            m_dragStartScreen   = { static_cast<float>(event.mouseButton.x),
                                    static_cast<float>(event.mouseButton.y) };
            m_dragStartOffset   = view.offset;
        }
        break;

    case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button == sf::Mouse::Left) {
            m_dragging = false;
        }
        break;

    case sf::Event::MouseMoved:
        if (m_dragging) {
            float dx = static_cast<float>(event.mouseMove.x) - m_dragStartScreen.x;
            float dy = static_cast<float>(event.mouseMove.y) - m_dragStartScreen.y;
            // Pan: screen delta → world delta (flip Y)
            view.offset.x = m_dragStartOffset.x - dx / view.scale;
            view.offset.y = m_dragStartOffset.y + dy / view.scale;
        }
        break;

    // ── Mouse scroll (zoom around cursor) ────────────────────────────────
    case sf::Event::MouseWheelScrolled:
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            float mouseX = static_cast<float>(event.mouseWheelScroll.x);
            float mouseY = static_cast<float>(event.mouseWheelScroll.y);

            // World position under cursor before zoom
            float worldX = view.offset.x + (mouseX - W / 2.0f) / view.scale;
            float worldY = view.offset.y - (mouseY - H / 2.0f) / view.scale;

            float factor = (event.mouseWheelScroll.delta > 0)
                           ? Constants::ZOOM_FACTOR
                           : (1.0f / Constants::ZOOM_FACTOR);
            float newScale = std::max(Constants::MIN_SCALE,
                             std::min(Constants::MAX_SCALE, view.scale * factor));

            // Adjust offset so the world point under the cursor stays fixed
            view.offset.x = worldX - (mouseX - W / 2.0f) / newScale;
            view.offset.y = worldY + (mouseY - H / 2.0f) / newScale;
            view.scale    = newScale;
        }
        break;

    // ── Resize ────────────────────────────────────────────────────────────
    case sf::Event::Resized:
        {
            sf::FloatRect visibleArea(0.f, 0.f,
                static_cast<float>(event.size.width),
                static_cast<float>(event.size.height));
            window.setView(sf::View(visibleArea));
        }
        break;

    default:
        break;
    }

    return true;
}
