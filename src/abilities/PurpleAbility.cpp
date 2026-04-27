// PurpleAbility.cpp
#include "PurpleAbility.h"
#include "systems/GameWorld.h"
#include "core/Constants.h"
#include "core/Utils.h"            // randFloat
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstdint>

PurpleAbility::PurpleAbility(GameWorld& world)
    : Ability(world)
{
    // orbs
    m_blueOrb.setRadius(20.f);
    m_blueOrb.setOrigin({ 20.f, 20.f });
    m_blueOrb.setFillColor(sf::Color(100, 150, 255, 220));

    m_redOrb.setRadius(20.f);
    m_redOrb.setOrigin({ 20.f, 20.f });
    m_redOrb.setFillColor(sf::Color(255, 80, 80, 220));

    // collision glow
    m_collisionGlowShape.setRadius(10.f);
    m_collisionGlowShape.setOrigin({ 10.f, 10.f });
    m_collisionGlowShape.setFillColor(sf::Color(220, 150, 255, 200));

    m_collisionRing.setFillColor(sf::Color::Transparent);
    m_collisionRing.setOutlineThickness(2.f);
    m_collisionRing.setOutlineColor(sf::Color(180, 100, 255, 255));

    // screen flash
    m_screenFlash.setSize({ WINDOW_WIDTH, WINDOW_HEIGHT });
    m_screenFlash.setPosition({ 0.f, 0.f });

    // sphere
    m_purpleSphere.setRadius(m_purpleRadius);
    m_purpleSphere.setOrigin({ m_purpleRadius, m_purpleRadius });
    m_purpleSphere.setFillColor(sf::Color(180, 100, 255, 200));
    m_purpleSphere.setOutlineThickness(4.f);
    m_purpleSphere.setOutlineColor(sf::Color(255, 150, 255, 180));
}

void PurpleAbility::update(float dt, sf::Vector2f mousePos, sf::Vector2f gojoPos) {
    // Input detection
    bool space = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
        sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);
    bool spaceRising = space && !m_spacePrev;
    bool spaceFalling = !space && m_spacePrev;
    m_spacePrev = space;
    // (Middle button is also covered by same flag; works fine)

    // Cooldown
    if (m_cooldown > 0.f) m_cooldown -= dt;

    // Start sequence on rising edge
    if (spaceRising && m_state == PurpleState::Idle && m_cooldown <= 0.f) {
        sf::Vector2f diff = mousePos - gojoPos;
        float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        m_direction = diff / len;
        sf::Vector2f perp = { -m_direction.y, m_direction.x };
        m_blueOrbPos = gojoPos + m_direction * 100.f - perp * 50.f;
        m_redOrbPos = gojoPos + m_direction * 100.f + perp * 50.f;
        m_mergePoint = gojoPos + m_direction * 150.f;
        m_collisionPoint = m_mergePoint;
        m_fireDirection = m_direction;
        m_blueOrbAlpha = 0.f;
        m_stateTimer = 1.f;
        m_state = PurpleState::BlueAppear;
    }

    // Cancel on release during early phases
    if (spaceFalling && (m_state == PurpleState::BlueAppear || m_state == PurpleState::RedAppear)) {
        m_state = PurpleState::Idle;
        m_blueOrbAlpha = 0.f;
        m_redOrbAlpha = 0.f;
    }

    // --- State machine (exactly as before, with world calls) ---
    if (m_state == PurpleState::BlueAppear) {
        m_blueOrbAlpha = std::min(m_blueOrbAlpha + 400.f * dt, 220.f);
        m_blueOrb.setFillColor(sf::Color(100, 150, 255, static_cast<uint8_t>(m_blueOrbAlpha)));
        m_blueOrb.setPosition(m_blueOrbPos);

        m_stateTimer -= dt;
        if (m_stateTimer <= 0.f) {
            m_redOrbAlpha = 0.f;
            m_stateTimer = 1.f;
            m_state = PurpleState::RedAppear;
        }
    }
    else if (m_state == PurpleState::RedAppear) {
        m_redOrbAlpha = std::min(m_redOrbAlpha + 400.f * dt, 220.f);
        m_redOrb.setFillColor(sf::Color(255, 80, 80, static_cast<uint8_t>(m_redOrbAlpha)));
        m_redOrb.setPosition(m_redOrbPos);
        m_blueOrb.setPosition(m_blueOrbPos);

        m_stateTimer -= dt;
        if (m_stateTimer <= 0.f) {
            m_stateTimer = 2.f;
            m_collisionGlow = 0.f;
            m_collisionPulse = 0.f;
            m_collisionRingRadius = 0.f;
            m_collisionRingAlpha = 0.f;
            m_state = PurpleState::Colliding;
            m_world.addScreenShake(8.f);
        }
    }
    else if (m_state == PurpleState::Colliding) {
        m_stateTimer -= dt;
        float progress = 1.f - (m_stateTimer / 2.f);

        m_blueOrbPos += (m_collisionPoint - m_blueOrbPos) * 5.f * dt;
        m_redOrbPos += (m_collisionPoint - m_redOrbPos) * 5.f * dt;
        m_blueOrb.setPosition(m_blueOrbPos);
        m_redOrb.setPosition(m_redOrbPos);

        m_collisionGlow = progress * 120.f;
        m_collisionPulse += dt * 6.f;
        float glowSize = 10.f + progress * 50.f + std::sin(m_collisionPulse) * 8.f;
        m_collisionGlowShape.setRadius(glowSize);
        m_collisionGlowShape.setOrigin({ glowSize, glowSize });
        m_collisionGlowShape.setPosition(m_collisionPoint);
        m_collisionGlowShape.setFillColor(sf::Color(220, 150, 255, static_cast<uint8_t>(m_collisionGlow)));

        m_collisionRingAlpha -= 300.f * dt;
        if (m_collisionRingAlpha <= 0.f) {
            m_collisionRingRadius = 0.f;
            m_collisionRingAlpha = 200.f;
        }
        m_collisionRingRadius += 150.f * dt;
        m_collisionRing.setRadius(m_collisionRingRadius);
        m_collisionRing.setOrigin({ m_collisionRingRadius, m_collisionRingRadius });
        m_collisionRing.setPosition(m_collisionPoint);
        m_collisionRing.setOutlineColor(sf::Color(180, 100, 255, static_cast<uint8_t>(m_collisionRingAlpha)));

        m_world.addScreenShake(5.f);   // subtle continuous

        if (m_stateTimer <= 0.f) {
            m_screenFlashAlpha = 255.f;
            m_stateTimer = 0.3f;
            m_state = PurpleState::ScreenFlash;
            m_world.addScreenShake(35.f);
        }
    }
    else if (m_state == PurpleState::ScreenFlash) {
        m_stateTimer -= dt;
        m_screenFlashAlpha = (m_stateTimer / 0.3f) * 255.f;
        m_screenFlash.setFillColor(sf::Color(180, 80, 255, static_cast<uint8_t>(m_screenFlashAlpha)));

        m_world.killInRadius(m_collisionPoint, 150.f);

        if (m_stateTimer <= 0.f) {
            m_purplePos = m_collisionPoint;
			m_purpleVelocity = m_fireDirection * 500.f; // initial speed
            m_purplePulse = 0.f;
            m_screenFlashAlpha = 0.f;
            m_stateTimer = .5f;
            m_state = PurpleState::Paused;
            m_world.addScreenShake(30.f);
        }
    }
    else if (m_state == PurpleState::Paused) {
        m_stateTimer -= dt;

        m_purplePulse += dt * 8.f;
        float chaos = randFloat(-2.f, 2.f);
        float r = m_purpleRadius + std::sin(m_purplePulse) * 12.f + chaos;
        m_purpleSphere.setRadius(r);
        m_purpleSphere.setOrigin({ r, r });
        m_purpleSphere.setPosition(m_purplePos);

        m_world.killInRadius(m_purplePos, r);
        m_world.purpleEnemyEffect(m_purplePos, r);
        m_world.addScreenShake(4.f);

        if (m_stateTimer <= 0.f)
            m_state = PurpleState::Travelling;
    }
    else if (m_state == PurpleState::Travelling) {
        m_purplePos += m_purpleVelocity * dt;

        m_purplePulse += dt * 12.f;
        float chaos = randFloat(-4.f, 4.f);
        float r = m_purpleRadius + std::sin(m_purplePulse) * 15.f + chaos;
        m_purpleSphere.setRadius(r);
        m_purpleSphere.setOrigin({ r, r });
        m_purpleSphere.setPosition(m_purplePos);

        m_world.killInRadius(m_purplePos, r);
        m_world.purpleEnemyEffect(m_purplePos, r);

        if (m_purplePos.x < -m_purpleRadius * 2 || m_purplePos.x > WINDOW_WIDTH + m_purpleRadius * 2 ||
            m_purplePos.y < -m_purpleRadius * 2 || m_purplePos.y > WINDOW_HEIGHT + m_purpleRadius * 2) {
            m_state = PurpleState::Idle;
            m_cooldown = COOLDOWN_DURATION;
            m_screenFlashAlpha = 0.f;
        }
    }
}

void PurpleAbility::draw(sf::RenderWindow& window) {
    if (m_state == PurpleState::BlueAppear)
        window.draw(m_blueOrb, sf::BlendAdd);

    if (m_state == PurpleState::RedAppear) {
        window.draw(m_blueOrb, sf::BlendAdd);
        window.draw(m_redOrb, sf::BlendAdd);
    }

    if (m_state == PurpleState::Colliding) {
        window.draw(m_blueOrb, sf::BlendAdd);
        window.draw(m_redOrb, sf::BlendAdd);
        window.draw(m_collisionGlowShape, sf::BlendAdd);
        window.draw(m_collisionRing, sf::BlendAdd);
    }

    if (m_state == PurpleState::ScreenFlash)
        window.draw(m_screenFlash);         // full overlay, no BlendAdd

    if (m_state == PurpleState::Paused || m_state == PurpleState::Travelling)
        window.draw(m_purpleSphere, sf::BlendAdd);
}