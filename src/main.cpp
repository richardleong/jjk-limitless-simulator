#include "Constants.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "Utils.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

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

    // Red ability visual
    sf::CircleShape redRing(0.f);
    redRing.setOrigin({ 0.f, 0.f });
    redRing.setFillColor(sf::Color::Transparent);
    redRing.setOutlineThickness(3.f);
    redRing.setOutlineColor(sf::Color(255, 80, 80, 255));
    float redRingRadius = 0.f;
    float redRingAlpha = 0.f;
    bool redActive = false;
    sf::Vector2f redOrigin;

    // Red charge up
    float redChargeTimer = 0.f;
    const float RED_CHARGE_DURATION = 0.4f;
    bool redCharging = false;

    sf::CircleShape redOrb(25.f);
    redOrb.setOrigin({ 25.f, 25.f });
    redOrb.setFillColor(sf::Color(255, 50, 50, 200));

    // screen shake
    sf::Vector2f shakeOffset(0.f, 0.f);
    float shakeIntensity = 0.f;
    sf::View view = window.getDefaultView();
    
    // track previous ring radius to call shockwave each frame
    float redRingPreviousRadius = 0.f;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyEvent->code == sf::Keyboard::Key::E && !redCharging)
                {
                    // start charge — don't fire yet
                    redCharging = true;
                    redChargeTimer = RED_CHARGE_DURATION;
                    redOrigin = mouse; // lock origin to where mouse was on press
                }
            }
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

        // "Update" Phase - crosshair to follow mouse
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

        // Red charge and explosion
        if (redCharging)
        {
            redChargeTimer -= dt;

            // orb grows during charge to show buildup
            float chargeProgress = 1.f - (redChargeTimer / RED_CHARGE_DURATION); // 0.0 to 1.0
            float orbSize = 15.f + chargeProgress * 40.f; // grows from 15px to 55px
            redOrb.setRadius(orbSize);
            redOrb.setOrigin({ orbSize, orbSize });
            redOrb.setPosition(redOrigin);

            if (redChargeTimer <= 0.f)
            {
                // charge complete — EXPLODE
                redCharging = false;
                redActive = true;
                redRingRadius = 0.f;
                redRingAlpha = 255.f;
                shakeIntensity = 25.f;
                particleSystem.applyRepulsion(redOrigin, 250000.f, 200.f); // stronger, more focused
            }
        }

        // screen shake decay
        if (shakeIntensity > 0.f)
        {
            shakeOffset = { randFloat(-shakeIntensity, shakeIntensity), randFloat(-shakeIntensity, shakeIntensity) };
            shakeIntensity -= 60.f * dt;
            if (shakeIntensity < 0.f) shakeIntensity = 0.f;
        }
        else shakeOffset = { 0.f, 0.f };

        // Red ring expand and fade
        if (redRingAlpha > 0.f)
        {
            redRingPreviousRadius = redRingRadius; // store previous radius
            redRingRadius += 400.f * dt;
            redRingAlpha -= 400.f * dt;

            // ring front pushes particles as it expands
            particleSystem.applyExpandingShockwave(redOrigin, redRingRadius, redRingPreviousRadius, 3000.f);

            redRing.setRadius(redRingRadius);
            redRing.setOrigin({ redRingRadius, redRingRadius });
            redRing.setPosition(redOrigin);
            redRing.setOutlineColor(sf::Color(255, 80, 80, (uint8_t)std::max(redRingAlpha, 0.f)));
        }
        else redRingPreviousRadius = 0.f; // reset when ring is gone

        // "Render" Phase
        view.setCenter({ WINDOW_WIDTH / 2.f + shakeOffset.x, WINDOW_HEIGHT / 2.f + shakeOffset.y });
        window.setView(view);
        window.clear(sf::Color(5, 5, 15)); // near black with slight blue tint
        window.draw(grid);
        particleSystem.draw(window);
        if (redCharging)
            window.draw(redOrb, sf::BlendAdd); // orb appears during charge
        if (redRingAlpha > 0.f)
            window.draw(redRing, sf::BlendAdd); // ring appears after explosion
        if (blueActive)
            window.draw(blueRing, sf::BlendAdd);
        window.draw(gojo);
        window.draw(crosshair);
        window.display();
    }

    return 0;
}