#pragma once
#include <SFML/Graphics.hpp>

class Particle
{
public:
	sf::CircleShape shape;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
    float radius;
    sf::Color colour;

    Particle();
    void respawn();
    void update(float dt);
    void draw(sf::RenderWindow& window);
};