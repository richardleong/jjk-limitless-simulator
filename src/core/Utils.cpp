#include "Utils.h"
#include <cmath>

std::mt19937& prng()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

float randFloat(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(prng());
}

void applyAttractionForce(sf::Vector2f& velocity, sf::Vector2f position,
    sf::Vector2f target, float strength, float dt) {
    float dx = target.x - position.x;
    float dy = target.y - position.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist < 1.f) return;
    float force = strength / (dist * dist + 30.f);
    velocity.x += (dx / dist) * force * dt;
    velocity.y += (dy / dist) * force * dt;
}

void applyRepulsionForce(sf::Vector2f& velocity, sf::Vector2f position,
    sf::Vector2f origin, float strength, float radius, float dt) {
    float dx = position.x - origin.x;
    float dy = position.y - origin.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist > radius || dist < 1.f) return;
    float falloff = 1.f - (dist / radius);
    float force = strength * falloff;
    velocity.x += (dx / dist) * force * dt;
    velocity.y += (dy / dist) * force * dt;
}

bool isInsideRadius(sf::Vector2f position, sf::Vector2f centre, float radius) {
    float dx = position.x - centre.x;
    float dy = position.y - centre.y;
    return (dx * dx + dy * dy) < (radius * radius);
}