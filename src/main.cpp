#include "core/Constants.h"
#include "systems/GameWorld.h"
#include "abilities/BlueAbility.h"
#include "abilities/RedAbility.h"
#include "abilities/PurpleAbility.h"
#include "core/Utils.h"
#include <SFML/Graphics.hpp>
#include <algorithm>

int main() {
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode({ (unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT }),
        "Gojo Limitless Simulator", sf::Style::Default, sf::State::Windowed, settings);
    window.setMouseCursorVisible(false);

    sf::Clock clock;

    // Gojo
    sf::CircleShape gojo(GOJO_RADIUS);
    gojo.setFillColor(sf::Color::White);
    gojo.setOrigin({ GOJO_RADIUS, GOJO_RADIUS });
    gojo.setPosition({ WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f });

    // Grid (unchanged)
    sf::VertexArray grid(sf::PrimitiveType::Lines);
    sf::Color gridColour(255, 255, 255, 20);
    for (float x = 0; x < WINDOW_WIDTH; x += GRID_SIZE) {
        grid.append(sf::Vertex{ {x, 0.f}, gridColour });
        grid.append(sf::Vertex{ {x, WINDOW_HEIGHT}, gridColour });
    }
    for (float y = 0; y < WINDOW_HEIGHT; y += GRID_SIZE) {
        grid.append(sf::Vertex{ {0.f, y}, gridColour });
        grid.append(sf::Vertex{ {WINDOW_WIDTH, y}, gridColour });
    }

    // Crosshair
    sf::VertexArray crosshair(sf::PrimitiveType::Lines, 4);
    float crossSize = 10.f;

    // World + Abilities
    GameWorld world;
    BlueAbility blue(world);
    RedAbility red(world);
    PurpleAbility purple(world);

    sf::View view = window.getDefaultView();

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        // Events
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            // Abilities don't rely on events, so nothing to forward
        }

        // Gojo movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            gojo.move({ 0.f, -GOJO_SPEED * dt });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            gojo.move({ 0.f, GOJO_SPEED * dt });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            gojo.move({ -GOJO_SPEED * dt, 0.f });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            gojo.move({ GOJO_SPEED * dt, 0.f });

        sf::Vector2f pos = gojo.getPosition();
        pos.x = std::clamp(pos.x, GOJO_RADIUS, WINDOW_WIDTH - GOJO_RADIUS);
        pos.y = std::clamp(pos.y, GOJO_RADIUS, WINDOW_HEIGHT - GOJO_RADIUS);
        gojo.setPosition(pos);

        // Update systems
        world.update(dt);                             // particles + shake
        blue.update(dt, mouse, gojo.getPosition());
        red.update(dt, mouse, gojo.getPosition());
        purple.update(dt, mouse, gojo.getPosition());

        sf::Vector2f shakeOffset = world.getShakeOffset();

        // Render
        view.setCenter({ WINDOW_WIDTH / 2.f + shakeOffset.x,
                         WINDOW_HEIGHT / 2.f + shakeOffset.y });
        window.setView(view);
        window.clear(sf::Color(5, 5, 15));
        window.draw(grid);
        world.draw(window);          // particles

        blue.draw(window);
        red.draw(window);
        purple.draw(window);

        window.draw(gojo);

        // Crosshair on top
        crosshair[0] = sf::Vertex{ {mouse.x - crossSize, mouse.y}, sf::Color::White };
        crosshair[1] = sf::Vertex{ {mouse.x + crossSize, mouse.y}, sf::Color::White };
        crosshair[2] = sf::Vertex{ {mouse.x, mouse.y - crossSize}, sf::Color::White };
        crosshair[3] = sf::Vertex{ {mouse.x, mouse.y + crossSize}, sf::Color::White };
        window.draw(crosshair);

        window.display();
    }

    return 0;
}