#include "Constants.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include <SFML/Graphics.hpp>
#include <algorithm>

int main()
{
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode({ (unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT }), "Gojo Limitless Simulator", sf::Style::Default, sf::State::Windowed, settings);
    window.setMouseCursorVisible(false); // hide default cursor

    sf::Clock clock;

    // Gojo placeholder — white glowing circle
    sf::CircleShape gojo(GOJO_RADIUS);
    gojo.setFillColor(sf::Color::White);
    gojo.setOrigin({ GOJO_RADIUS, GOJO_RADIUS }); // centre origin so position = centre
    gojo.setPosition({ WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f }); // start at centre

    // precompute grid here once, before the loop
    sf::VertexArray grid(sf::PrimitiveType::Lines);
    sf::Color gridColour(255, 255, 255, 20);

    // crosshair — two lines forming a +
    sf::VertexArray crosshair(sf::PrimitiveType::Lines, 4);

    // particle system
	ParticleSystem particleSystem(500);

    for (float x = 0; x < WINDOW_WIDTH; x += GRID_SIZE)
    {
        grid.append(sf::Vertex{ {x, 0.f}, gridColour });
        grid.append(sf::Vertex{ {x, WINDOW_HEIGHT}, gridColour });
    }
    for (float y = 0; y < WINDOW_HEIGHT; y += GRID_SIZE)
    {
        grid.append(sf::Vertex{ {0.f, y}, gridColour });
        grid.append(sf::Vertex{ {WINDOW_WIDTH, y}, gridColour });
    }

    // Blue ability visual
    sf::CircleShape blueRing(40.f);
    blueRing.setOrigin({ 40.f, 40.f });
    blueRing.setFillColor(sf::Color::Transparent);
    blueRing.setOutlineThickness(2.f);
    blueRing.setOutlineColor(sf::Color(100, 150, 255, 180));

    float blueRingPulse = 0.f; // tracks pulse animation
    bool blueActive = false;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // WASD movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            gojo.move({ 0.f, -GOJO_SPEED * dt });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            gojo.move({ 0.f, GOJO_SPEED * dt });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            gojo.move({ -GOJO_SPEED * dt, 0.f });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            gojo.move({ GOJO_SPEED * dt, 0.f });

        // clamp Gojo to window bounds
        sf::Vector2f pos = gojo.getPosition();
        pos.x = std::clamp(pos.x, GOJO_RADIUS, WINDOW_WIDTH - GOJO_RADIUS);
        pos.y = std::clamp(pos.y, GOJO_RADIUS, WINDOW_HEIGHT - GOJO_RADIUS);
        gojo.setPosition(pos);

        particleSystem.update(dt);

        // update crosshair to follow mouse
        sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        float crossSize = 10.f;
        crosshair[0] = sf::Vertex{ {mouse.x - crossSize, mouse.y}, sf::Color::White };
        crosshair[1] = sf::Vertex{ {mouse.x + crossSize, mouse.y}, sf::Color::White };
        crosshair[2] = sf::Vertex{ {mouse.x, mouse.y - crossSize}, sf::Color::White };
        crosshair[3] = sf::Vertex{ {mouse.x, mouse.y + crossSize}, sf::Color::White };

        // Blue ability
        blueActive = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q);
        if (blueActive)
        {
            particleSystem.applyAttraction(mouse, 80000.f);

            // pulse ring size
            blueRingPulse += dt * 3.f;
            float pulseSize = 40.f + std::sin(blueRingPulse) * 10.f;
            blueRing.setRadius(pulseSize);
            blueRing.setOrigin({ pulseSize, pulseSize });
            blueRing.setPosition(mouse);
        }

        // render
        window.clear(sf::Color(5, 5, 15)); // near black with slight blue tint
        window.draw(grid);
        particleSystem.draw(window);
        if (blueActive)
            window.draw(blueRing, sf::BlendAdd);
        window.draw(gojo);
        window.draw(crosshair);
        window.display();
    }

    return 0;
}