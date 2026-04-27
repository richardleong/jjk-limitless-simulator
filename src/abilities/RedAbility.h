// RedAbility.h
#pragma once
#include "Ability.h"

class RedAbility : public Ability {
    bool m_charging = false;
    float m_chargeTimer = 0.f;
    static constexpr float CHARGE_DURATION = 0.4f;
    sf::Vector2f m_origin;

    // ring explosion
    float m_ringRadius = 0.f;
    float m_ringAlpha = 0.f;
    float m_ringPrevRadius = 0.f;
    sf::CircleShape m_ring;

    // charging orb
    sf::CircleShape m_orb;

    // input rising edge detection
    bool m_prevPress = false;

public:
    RedAbility(GameWorld& world);

    void update(float dt, sf::Vector2f mousePos, sf::Vector2f gojoPos) override;
    void draw(sf::RenderWindow& window) override;
    bool isCharging() const { return m_charging; }
};