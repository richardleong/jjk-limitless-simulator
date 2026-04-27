// GameWorld.cpp
#include "GameWorld.h"

GameWorld::GameWorld()
    : m_particleSystem(300)          // 300 particles as before
{
}

void GameWorld::update(float dt) {
    m_particleSystem.update(dt);
    m_screenShake.update(dt);
}

void GameWorld::draw(sf::RenderWindow& window) {
    m_particleSystem.draw(window);
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