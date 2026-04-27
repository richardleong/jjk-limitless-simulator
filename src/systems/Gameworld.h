// GameWorld.h
#pragma once
#include "gameplay/ParticleSystem.h"
#include "ScreenShake.h"
#include "gameplay/EnemyManager.h"
#include <SFML/Graphics.hpp>

class GameWorld {
    ParticleSystem m_particleSystem;
    ScreenShake    m_screenShake;
    EnemyManager   m_enemyManager;
public:
    GameWorld();

    void update(float dt, sf::Vector2f gojoPos);   // <-- changed signature
    void draw(sf::RenderWindow& window);

    // Particle helpers (unchanged)
    void applyAttraction(sf::Vector2f target, float strength);
    void applyRepulsion(sf::Vector2f origin, float strength, float killRadius);
    void applyExpandingShockwave(sf::Vector2f origin, float currentRadius,
        float previousRadius, float strength);
    void killInRadius(sf::Vector2f pos, float radius);

    // Screen shake
    void addScreenShake(float intensity);
    sf::Vector2f getShakeOffset() const;

    // Enemy ability hooks (new)
    void attractEnemies(sf::Vector2f target, float strength);
    void blueEnemyEffect(sf::Vector2f centre, float innerRadius, float damageRadius, float dt);
    void redEnemyEffect(sf::Vector2f origin, float strength, float killRadius);
    void purpleEnemyEffect(sf::Vector2f centre, float radius);

    void spawnEnemyWave(EnemyType type, int count);
};