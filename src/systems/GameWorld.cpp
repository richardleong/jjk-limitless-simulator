// GameWorld.cpp
#include "GameWorld.h"
#include "core/Constants.h"
#include "core/Utils.h"
#include "gameplay/EnemyManager.h"

GameWorld::GameWorld()
    : m_particleSystem(300)
{
    // Spawn waves of enemies — tweak these numbers freely
    spawnEnemyWave(EnemyType::Weak, 5);
    spawnEnemyWave(EnemyType::Basic, 3);
    spawnEnemyWave(EnemyType::Anchor, 2);
    spawnEnemyWave(EnemyType::Blink, 2);
}

void GameWorld::spawnEnemyWave(EnemyType type, int count) {
    for (int i = 0; i < count; ++i) {
        sf::Vector2f pos(
            randFloat(100.f, WINDOW_WIDTH - 100.f),
            randFloat(100.f, WINDOW_HEIGHT - 100.f)
        );
        m_enemyManager.spawnEnemy(type, pos);
    }
}
void GameWorld::update(float dt, sf::Vector2f gojoPos) {
    m_particleSystem.update(dt);
    m_screenShake.update(dt);
    m_enemyManager.update(dt, gojoPos);
}

void GameWorld::draw(sf::RenderWindow& window) {
    m_particleSystem.draw(window);
    m_enemyManager.draw(window);
}

void GameWorld::applyAttraction(sf::Vector2f target, float strength) {
    m_particleSystem.applyAttraction(target, strength);
}

void GameWorld::applyRepulsion(sf::Vector2f origin, float strength, float killRadius) {
    m_particleSystem.applyRepulsion(origin, strength, killRadius);
}

void GameWorld::applyExpandingShockwave(sf::Vector2f origin, float currentRadius,
    float previousRadius, float strength) {
    m_particleSystem.applyExpandingShockwave(origin, currentRadius, previousRadius, strength);
}

void GameWorld::killInRadius(sf::Vector2f pos, float radius) {
    m_particleSystem.killInRadius(pos, radius);
}

void GameWorld::addScreenShake(float intensity) {
    m_screenShake.addShake(intensity);
}

sf::Vector2f GameWorld::getShakeOffset() const {
    return m_screenShake.getOffset();
}

void GameWorld::attractEnemies(sf::Vector2f target, float strength) {
    m_enemyManager.attractEnemies(target, strength);
}

void GameWorld::blueEnemyEffect(sf::Vector2f centre, float innerRadius, float damageRadius, float dt) {
    m_enemyManager.applyBlueEffect(centre, innerRadius, damageRadius, dt);
}

void GameWorld::redEnemyEffect(sf::Vector2f origin, float strength, float killRadius) {
    m_enemyManager.applyRedEffect(origin, strength, killRadius);
}

void GameWorld::purpleEnemyEffect(sf::Vector2f centre, float radius) {
    m_enemyManager.applyPurpleEffect(centre, radius);
}