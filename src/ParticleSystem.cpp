#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(int count)
{
    particles.resize(count);
}

void ParticleSystem::update(float dt)
{
    for (auto& p : particles)
        p.update(dt);
}

void ParticleSystem::draw(sf::RenderWindow& window)
{
    for (auto& p : particles)
        p.draw(window);
}