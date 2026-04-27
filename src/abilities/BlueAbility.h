// BlueAbility.h
#pragma once
#include "Ability.h"

class BlueAbility : public Ability {
    bool m_active = false;
    float m_pulse = 0.f;
    sf::CircleShape m_ring;
public:
    BlueAbility(GameWorld& world);
    void update(float dt, sf::Vector2f mousePos, sf::Vector2f gojoPos) override;
    void draw(sf::RenderWindow& window) override;
};