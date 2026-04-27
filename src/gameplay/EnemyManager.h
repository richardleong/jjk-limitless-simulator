#pragma once
#include "Enemy.h"
#include <vector>
#include <SFML/Graphics.hpp>

class EnemyManager {
    std::vector<Enemy> m_enemies;
public:
    void spawnEnemy(EnemyType type, sf::Vector2f pos);
    void update(float dt, sf::Vector2f gojoPos);
    void draw(sf::RenderWindow& window);

    // ability helpers
    void applyForce(sf::Vector2f force);  // uniform force to all
    void applyRadialForce(sf::Vector2f origin, float strength, float radius);
    void killInRadius(sf::Vector2f centre, float radius);
    void applyBlueEffect(sf::Vector2f centre, float innerRadius, float damageRadius, float dt);
    void applyRedEffect(sf::Vector2f origin, float strength, float killRadius);
    void applyPurpleEffect(sf::Vector2f centre, float radius);
    void attractEnemies(sf::Vector2f target, float strength);

    const std::vector<Enemy>& getEnemies() const { return m_enemies; }
    void clearDead();
};