#include "Constants.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "Utils.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

enum class PurpleState
{
    Idle,
    BlueAppear,
    RedAppear,
	Colliding,
	ScreenFlash,
    Paused,
	Travelling
};

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
	ParticleSystem particleSystem(300);

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

    // Purple state machine
    PurpleState purpleState = PurpleState::Idle;
    float purpleStateTimer = 0.f;
    float purpleCooldown = 0.f;
    const float PURPLE_COOLDOWN_DURATION = 2.f;

    // direction locked when Space first pressed
    sf::Vector2f purpleDirection;
    sf::Vector2f purpleFireDirection; // locked at collision, used for travel

    // Blue and Red orbs for merge sequence
    sf::Vector2f blueOrbPos;
    sf::Vector2f redOrbPos;
    sf::Vector2f mergePoint;
    float blueOrbAlpha = 0.f;
    float redOrbAlpha = 0.f;

    sf::CircleShape purpleBlueOrb(20.f);
    purpleBlueOrb.setOrigin({ 20.f, 20.f });
    purpleBlueOrb.setFillColor(sf::Color(100, 150, 255, 220));

    sf::CircleShape purpleRedOrb(20.f);
    purpleRedOrb.setOrigin({ 20.f, 20.f });
    purpleRedOrb.setFillColor(sf::Color(255, 80, 80, 220));

    // collision point effects
    sf::Vector2f collisionPoint;
    float collisionGlow = 0.f;
    float collisionPulse = 0.f;
    float collisionRingRadius = 0.f;
    float collisionRingAlpha = 0.f;

    sf::CircleShape collisionGlowShape(10.f);
    collisionGlowShape.setOrigin({ 10.f, 10.f });
    collisionGlowShape.setFillColor(sf::Color(220, 150, 255, 200));

    sf::CircleShape collisionRing(0.f);
    collisionRing.setFillColor(sf::Color::Transparent);
    collisionRing.setOutlineThickness(2.f);
    collisionRing.setOutlineColor(sf::Color(180, 100, 255, 255));

    // screen flash overlay
    float screenFlashAlpha = 0.f;
    sf::RectangleShape screenFlash(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    screenFlash.setPosition({ 0.f, 0.f });

    // Purple sphere
    float purpleRadius = 80.f;
    float purplePulse = 0.f;
    sf::Vector2f purplePos;
    sf::Vector2f purpleVelocity;

    sf::CircleShape purpleSphere(purpleRadius);
    purpleSphere.setOrigin({ purpleRadius, purpleRadius });
    purpleSphere.setFillColor(sf::Color(180, 100, 255, 200));
    purpleSphere.setOutlineThickness(4.f);
    purpleSphere.setOutlineColor(sf::Color(255, 150, 255, 180));

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

                // Space PRESSED — start sequence
                if (keyEvent->code == sf::Keyboard::Key::Space &&
                    purpleState == PurpleState::Idle && purpleCooldown <= 0.f)
                {
                    sf::Vector2f gojoPos = gojo.getPosition();
                    sf::Vector2f diff = mouse - gojoPos;
                    float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                    purpleDirection = { diff.x / len, diff.y / len };

                    sf::Vector2f perp = { -purpleDirection.y, purpleDirection.x };
                    blueOrbPos = gojoPos + purpleDirection * 100.f - perp * 50.f;
                    redOrbPos = gojoPos + purpleDirection * 100.f + perp * 50.f;
                    mergePoint = gojoPos + purpleDirection * 150.f;
                    collisionPoint = mergePoint;
                    purpleFireDirection = purpleDirection; // lock direction

                    blueOrbAlpha = 0.f;
                    purpleStateTimer = 1.f; // Blue appear phase lasts 1s
                    purpleState = PurpleState::BlueAppear;
                }
            }
            if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
            {
                if (keyReleased->code == sf::Keyboard::Key::Space)
                {
                    // cancel if released before Colliding — no cooldown
                    if (purpleState == PurpleState::BlueAppear ||
                        purpleState == PurpleState::RedAppear)
                    {
                        purpleState = PurpleState::Idle;
                        blueOrbAlpha = 0.f;
                        redOrbAlpha = 0.f;
                    }
                }
            }
            if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (mouseReleased->button == sf::Mouse::Button::Middle)
                {
                    if (purpleState == PurpleState::BlueAppear ||
                        purpleState == PurpleState::RedAppear)
                    {
                        purpleState = PurpleState::Idle;
                        blueOrbAlpha = 0.f;
                        redOrbAlpha = 0.f;
                    }
                }
            }

            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>())
            {
                // Left click — Blue (same as Q)
                if (mouseEvent->button == sf::Mouse::Button::Left)
                {
                    // Blue is hold-based so we just set a flag
                    // handled in update phase via flag
                }

                // Right click — Red (same as E)
                if (mouseEvent->button == sf::Mouse::Button::Right && !redCharging)
                {
                    redCharging = true;
                    redChargeTimer = RED_CHARGE_DURATION;
                    redOrigin = mouse;
                }

                // Middle click — Purple (same as Space)
                if (mouseEvent->button == sf::Mouse::Button::Middle &&
                    purpleState == PurpleState::Idle && purpleCooldown <= 0.f)
                {
                    sf::Vector2f gojoPos = gojo.getPosition();
                    sf::Vector2f diff = mouse - gojoPos;
                    float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                    purpleDirection = { diff.x / len, diff.y / len };

                    sf::Vector2f perp = { -purpleDirection.y, purpleDirection.x };
                    blueOrbPos = gojoPos + purpleDirection * 100.f - perp * 50.f;
                    redOrbPos = gojoPos + purpleDirection * 100.f + perp * 50.f;
                    mergePoint = gojoPos + purpleDirection * 150.f;
                    collisionPoint = mergePoint;
                    purpleFireDirection = purpleDirection;

                    blueOrbAlpha = 0.f;
                    purpleStateTimer = 1.f;
                    purpleState = PurpleState::BlueAppear;
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
        blueActive = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) ||
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
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

        // Purple cooldown
        if (purpleCooldown > 0.f)
            purpleCooldown -= dt;

        // Purple state machine
        if (purpleState == PurpleState::BlueAppear)
        {
            // cancel if Space released — handled in event loop
            // fade Blue orb in
            blueOrbAlpha = std::min(blueOrbAlpha + 400.f * dt, 220.f);
            purpleBlueOrb.setFillColor(sf::Color(100, 150, 255, (uint8_t)blueOrbAlpha));
            purpleBlueOrb.setPosition(blueOrbPos);

            purpleStateTimer -= dt;
            if (purpleStateTimer <= 0.f)
            {
                redOrbAlpha = 0.f;
                purpleStateTimer = 1.f; // Red appear phase lasts 1s
                purpleState = PurpleState::RedAppear;
            }
        }
        else if (purpleState == PurpleState::RedAppear)
        {
            // fade Red orb in
            redOrbAlpha = std::min(redOrbAlpha + 400.f * dt, 220.f);
            purpleRedOrb.setFillColor(sf::Color(255, 80, 80, (uint8_t)redOrbAlpha));
            purpleRedOrb.setPosition(redOrbPos);

            // keep Blue visible
            purpleBlueOrb.setPosition(blueOrbPos);

            purpleStateTimer -= dt;
            if (purpleStateTimer <= 0.f)
            {
                purpleStateTimer = 2.f; // Colliding phase lasts 2s
                collisionGlow = 0.f;
                collisionPulse = 0.f;
                collisionRingRadius = 0.f;
                collisionRingAlpha = 0.f;
                purpleState = PurpleState::Colliding;
                shakeIntensity = 8.f;
            }
        }
        else if (purpleState == PurpleState::Colliding)
        {
            purpleStateTimer -= dt;
            float progress = 1.f - (purpleStateTimer / 2.f); // 0 to 1 over 2s

            // orbs rush toward collision point
            blueOrbPos = blueOrbPos + (collisionPoint - blueOrbPos) * 5.f * dt;
            redOrbPos = redOrbPos + (collisionPoint - redOrbPos) * 5.f * dt;
            purpleBlueOrb.setPosition(blueOrbPos);
            purpleRedOrb.setPosition(redOrbPos);

            // collision glow grows brighter over 2s
            collisionGlow = progress * 120.f;
            collisionPulse += dt * 6.f;
            float glowSize = 10.f + progress * 50.f + std::sin(collisionPulse) * 8.f;
            collisionGlowShape.setRadius(glowSize);
            collisionGlowShape.setOrigin({ glowSize, glowSize });
            collisionGlowShape.setPosition(collisionPoint);
            collisionGlowShape.setFillColor(sf::Color(220, 150, 255, (uint8_t)collisionGlow));

            // periodic shockwave rings from collision point
            collisionRingAlpha -= 300.f * dt;
            if (collisionRingAlpha <= 0.f)
            {
                // emit new ring
                collisionRingRadius = 0.f;
                collisionRingAlpha = 200.f;
            }
            collisionRingRadius += 150.f * dt;
            collisionRing.setRadius(collisionRingRadius);
            collisionRing.setOrigin({ collisionRingRadius, collisionRingRadius });
            collisionRing.setPosition(collisionPoint);
            collisionRing.setOutlineColor(sf::Color(180, 100, 255, (uint8_t)collisionRingAlpha));

            // continuous subtle shake
            shakeIntensity = std::max(shakeIntensity, 5.f);

            if (purpleStateTimer <= 0.f)
            {
                // transition to screen flash
                screenFlashAlpha = 255.f;
                purpleStateTimer = 0.3f;
                purpleState = PurpleState::ScreenFlash;
                shakeIntensity = 35.f;
            }
        }
        else if (purpleState == PurpleState::ScreenFlash)
        {
            purpleStateTimer -= dt;
            screenFlashAlpha = (purpleStateTimer / 0.3f) * 255.f;
            screenFlash.setFillColor(sf::Color(180, 80, 255, (uint8_t)screenFlashAlpha));

            // kill particles near collision point during flash
            particleSystem.killInRadius(collisionPoint, 150.f);

            if (purpleStateTimer <= 0.f)
            {
                // spawn Purple sphere
                purplePos = collisionPoint;
                purpleVelocity = purpleFireDirection * 800.f;
                purplePulse = 0.f;
                screenFlashAlpha = 0.f;
				purpleStateTimer = 3.f; // Paused phase timer
                purpleState = PurpleState::Paused;
                shakeIntensity = 30.f;
            }
        }
        else if (purpleState == PurpleState::Paused)
        {
            purpleStateTimer -= dt;

            // pulsating aura while paused
            purplePulse += dt * 8.f;
            float chaos = randFloat(-2.f, 2.f);
            float currentRadius = purpleRadius + std::sin(purplePulse) * 12.f + chaos;
            purpleSphere.setRadius(currentRadius);
            purpleSphere.setOrigin({ currentRadius, currentRadius });
            purpleSphere.setPosition(purplePos);

            // kill particles nearby during aura
            particleSystem.killInRadius(purplePos, currentRadius);

            // subtle continuous shake during pause
            shakeIntensity = std::max(shakeIntensity, 4.f);

            if (purpleStateTimer <= 0.f)
                purpleState = PurpleState::Travelling;
                }
        else if (purpleState == PurpleState::Travelling)
        {
            purplePos += purpleVelocity * dt;

            // chaotic pulsating size
            purplePulse += dt * 12.f;
            float chaos = randFloat(-4.f, 4.f);
            float currentRadius = purpleRadius + std::sin(purplePulse) * 15.f + chaos;
            purpleSphere.setRadius(currentRadius);
            purpleSphere.setOrigin({ currentRadius, currentRadius });
            purpleSphere.setPosition(purplePos);

            // erase particles in path
            particleSystem.killInRadius(purplePos, currentRadius);

            // check if exited screen
            if (purplePos.x < -purpleRadius * 2 || purplePos.x > WINDOW_WIDTH + purpleRadius * 2 ||
                purplePos.y < -purpleRadius * 2 || purplePos.y > WINDOW_HEIGHT + purpleRadius * 2)
            {
                purpleState = PurpleState::Idle;
                purpleCooldown = PURPLE_COOLDOWN_DURATION;
                screenFlashAlpha = 0.f;
            }
        }

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
        // Purple merge sequence
        if (purpleState == PurpleState::BlueAppear)
            window.draw(purpleBlueOrb, sf::BlendAdd);

        if (purpleState == PurpleState::RedAppear)
        {
            window.draw(purpleBlueOrb, sf::BlendAdd);
            window.draw(purpleRedOrb, sf::BlendAdd);
        }

        if (purpleState == PurpleState::Colliding)
        {
            window.draw(purpleBlueOrb, sf::BlendAdd);
            window.draw(purpleRedOrb, sf::BlendAdd);
            window.draw(collisionGlowShape, sf::BlendAdd);
            window.draw(collisionRing, sf::BlendAdd);
        }

        if (purpleState == PurpleState::ScreenFlash)
            window.draw(screenFlash); // no BlendAdd — full overlay
        
        if (purpleState == PurpleState::Paused || purpleState == PurpleState::Travelling)
            window.draw(purpleSphere, sf::BlendAdd);

        window.draw(gojo);
        window.draw(crosshair);
        window.display();
    }

    return 0;
}