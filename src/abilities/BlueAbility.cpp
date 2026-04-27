// BlueAbility.cpp
#include "BlueAbility.h"
#include "systems/GameWorld.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

BlueAbility::BlueAbility(GameWorld& world)
    : Ability(world)
{
    m_ring.setRadius(40.f);
    m_ring.setOrigin({ 40.f, 40.f });
    m_ring.setFillColor(sf::Color::Transparent);
    m_ring.setOutlineThickness(2.f);
    m_ring.setOutlineColor(sf::Color(100, 150, 255, 180));
}

void BlueAbility::update(float dt, sf::Vector2f mousePos, sf::Vector2f) {
    m_active = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) ||
        sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (m_active) {
        m_world.applyAttraction(mousePos, 80000.f);

        // Attract enemies (low strength to let resistance work)
        m_world.attractEnemies(mousePos, 4000.f);
        // Apply the "obliteration" effect
        m_world.blueEnemyEffect(mousePos, 60.f, 100.f, dt);

        m_pulse += dt * 3.f;
        float size = 40.f + std::sin(m_pulse) * 10.f;
        m_ring.setRadius(size);
        m_ring.setOrigin({ size, size });
        m_ring.setPosition(mousePos);
    }
}

void BlueAbility::draw(sf::RenderWindow& window) {
    if (m_active)
        window.draw(m_ring, sf::BlendAdd);
}