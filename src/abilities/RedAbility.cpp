// RedAbility.cpp
#include "RedAbility.h"
#include "systems/GameWorld.h"
#include "core/Constants.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstdint>

RedAbility::RedAbility(GameWorld& world)
    : Ability(world)
{
    // ring
    m_ring.setFillColor(sf::Color::Transparent);
    m_ring.setOutlineThickness(3.f);
    m_ring.setOutlineColor(sf::Color(255, 80, 80, 255));

    // orb
    m_orb.setOrigin({ 25.f, 25.f });   // will be resized during charge
    m_orb.setFillColor(sf::Color(255, 50, 50, 200));
}

void RedAbility::update(float dt, sf::Vector2f mousePos, sf::Vector2f) {
    bool press = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E) ||
        sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);

    // Start charge on rising edge
    if (press && !m_prevPress && !m_charging) {
        m_charging = true;
        m_chargeTimer = CHARGE_DURATION;
        m_origin = mousePos;
    }
    m_prevPress = press;

    // Charge phase
    if (m_charging) {
        m_chargeTimer -= dt;
        float progress = 1.f - (m_chargeTimer / CHARGE_DURATION);
        float orbSize = 15.f + progress * 40.f;
        m_orb.setRadius(orbSize);
        m_orb.setOrigin({ orbSize, orbSize });
        m_orb.setPosition(m_origin);

        if (m_chargeTimer <= 0.f) {
            // Explode
            m_charging = false;
            m_ringRadius = 0.f;
            m_ringAlpha = 255.f;
            m_ringPrevRadius = 0.f;
            m_world.applyRepulsion(m_origin, 250000.f, 200.f);
            m_world.addScreenShake(25.f);
        }
    }

    // Ring expansion
    if (m_ringAlpha > 0.f) {
        m_ringPrevRadius = m_ringRadius;
        m_ringRadius += 400.f * dt;
        m_ringAlpha -= 400.f * dt;

        m_world.applyExpandingShockwave(m_origin, m_ringRadius, m_ringPrevRadius, 3000.f);

        m_ring.setRadius(m_ringRadius);
        m_ring.setOrigin({ m_ringRadius, m_ringRadius });
        m_ring.setPosition(m_origin);
        m_ring.setOutlineColor(sf::Color(255, 80, 80,
            static_cast<uint8_t>(std::max(m_ringAlpha, 0.f))));
    }
}

void RedAbility::draw(sf::RenderWindow& window) {
    if (m_charging)
        window.draw(m_orb, sf::BlendAdd);
    if (m_ringAlpha > 0.f)
        window.draw(m_ring, sf::BlendAdd);
}