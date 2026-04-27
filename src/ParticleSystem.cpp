#include "ParticleSystem.h"
#include <cmath>

ParticleSystem::ParticleSystem(int count)
{
    particles.resize(count);
}

void ParticleSystem::update(float dt)
{
    for (auto& p : particles)
    {
        p.beingPulled = false;
        p.update(dt);
    }
}

void ParticleSystem::draw(sf::RenderWindow& window)
{
    for (auto& p : particles)
        p.draw(window);
}

void ParticleSystem::applyAttraction(sf::Vector2f target, float strength)
{
    for (auto& p : particles)
    {
        float dx = target.x - p.position.x;
        float dy = target.y - p.position.y;

        float distance = std::sqrt(dx * dx + dy * dy);
        float force = strength / (distance * distance + 30.f); // epsilon smoothing

        p.velocity.x += (dx / distance) * force;
        p.velocity.y += (dy / distance) * force;
        p.beingPulled = true;
    }
}