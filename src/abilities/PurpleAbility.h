// PurpleAbility.h
#pragma once
#include "Ability.h"
#include <SFML/Graphics.hpp>

enum class PurpleState {
    Idle,
    BlueAppear,
    RedAppear,
    Colliding,
    ScreenFlash,
    Paused,
    Travelling
};

class PurpleAbility : public Ability {
    PurpleState m_state = PurpleState::Idle;
    float m_stateTimer = 0.f;
    float m_cooldown = 0.f;
    static constexpr float COOLDOWN_DURATION = 2.f;

    sf::Vector2f m_direction;
    sf::Vector2f m_fireDirection;
    sf::Vector2f m_blueOrbPos, m_redOrbPos;
    sf::Vector2f m_mergePoint;
    float m_blueOrbAlpha = 0.f, m_redOrbAlpha = 0.f;

    sf::CircleShape m_blueOrb;
    sf::CircleShape m_redOrb;

    sf::Vector2f m_collisionPoint;
    float m_collisionGlow = 0.f;
    float m_collisionPulse = 0.f;
    float m_collisionRingRadius = 0.f;
    float m_collisionRingAlpha = 0.f;
    sf::CircleShape m_collisionGlowShape;
    sf::CircleShape m_collisionRing;

    float m_screenFlashAlpha = 0.f;
    sf::RectangleShape m_screenFlash;

    float m_purpleRadius = 80.f;
    float m_purplePulse = 0.f;
    sf::Vector2f m_purplePos;
    sf::Vector2f m_purpleVelocity;
    sf::CircleShape m_purpleSphere;

    bool m_spacePrev = false;

public:
    PurpleAbility(GameWorld& world);

    void update(float dt, sf::Vector2f mousePos, sf::Vector2f gojoPos) override;
    void draw(sf::RenderWindow& window) override;
};