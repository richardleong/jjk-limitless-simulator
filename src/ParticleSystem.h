#pragma once
#include "Particle.h"
#include <vector>

class ParticleSystem
{
public:
    std::vector<Particle> particles;

    ParticleSystem(int count);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void applyAttraction(sf::Vector2f target, float strength);
	void applyRepulsion(sf::Vector2f origin, float strength, float radius);
    void applyExpandingShockwave(sf::Vector2f origin, float currentRadius, float previousRadius, float strength);
	void killInRadius(sf::Vector2f origin, float radius);
};