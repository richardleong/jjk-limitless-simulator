// Ability.h
#pragma once
#include <SFML/Graphics.hpp>

class GameWorld;

class Ability {
protected:
    GameWorld& m_world;
public:
    Ability(GameWorld& world) : m_world(world) {}
    virtual ~Ability() = default;

    virtual void handleEvent(const sf::Event& event) {}
    virtual void update(float dt, sf::Vector2f mousePos, sf::Vector2f gojoPos) {}
    virtual void draw(sf::RenderWindow& window) {}
};