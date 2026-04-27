// GameWorld.h
#pragma once
#include "gameplay/ParticleSystem.h"
#include "ScreenShake.h"
#include <SFML/Graphics.hpp>

class GameWorld {
    ParticleSystem m_particleSystem;
    ScreenShake    m_screenShake;
public:
    GameWorld();

    void update(float dt);
    void draw(sf::RenderWindow& window);

    // Force helpers – delegate to particle system
    void applyAttraction(sf::Vector2f target, float strength);
    void applyRepulsion(sf::Vector2f origin, float strength, float killRadius);
    void applyExpandingShockwave(sf::Vector2f origin, float currentRadius,
        float previousRadius, float strength);
    void killInRadius(sf::Vector2f pos, float radius);

    // Screen shake
    void addScreenShake(float intensity);
    sf::Vector2f getShakeOffset() const;
};