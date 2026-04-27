#include "ParticleSystem.h"
#include "core/Utils.h"
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
        applyAttractionForce(p.velocity, p.position, target, strength, 1.f); // dt=1.f for particles since they update separately
        p.beingPulled = true;
    }
}

void ParticleSystem::applyRepulsion(sf::Vector2f origin, float strength, float radius)
{
    for (auto& p : particles)
    {
        float dx = p.position.x - origin.x;
        float dy = p.position.y - origin.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance > radius || distance < 1.f) continue;

        if (distance < 20.f)
        {
            p.respawn();
            continue;
        }

        applyRepulsionForce(p.velocity, p.position, origin, strength, radius, 1.f);

        // Upgrade C — break effect on impact
        p.velocity.x += randFloat(-150.f, 150.f); // random jitter
        p.velocity.y += randFloat(-150.f, 150.f);
        p.colour = sf::Color(255, 80, 80); // flash red
    }
}

void ParticleSystem::applyExpandingShockwave(sf::Vector2f origin, float currentRadius, float previousRadius, float strength)
{
    for (auto& p : particles)
    {
        float dx = p.position.x - origin.x;
        float dy = p.position.y - origin.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        // only affect particles that the ring front just passed over this frame
        if (distance < previousRadius || distance > currentRadius) continue;

        // push outward
        float force = strength / (distance + 1.f);
        p.velocity.x += (dx / distance) * force;
        p.velocity.y += (dy / distance) * force;

        // small jitter for tear effect
        p.velocity.x += randFloat(-80.f, 80.f);
        p.velocity.y += randFloat(-80.f, 80.f);
    }
}

void ParticleSystem::killInRadius(sf::Vector2f origin, float radius)
{
    for (auto& p : particles)
    {
        if (isInsideRadius(p.position, origin, radius))
        {
            float dx = p.position.x - origin.x;
            float dy = p.position.y - origin.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            // compress inward before deletion — pulls toward centre briefly
            float nx = dx / (distance + 1.f);
            float ny = dy / (distance + 1.f);
            p.velocity.x -= nx * 300.f;
            p.velocity.y -= ny * 300.f;
            p.lifetime = std::min(p.lifetime, 0.1f); // force fast fade
        }
    }
}