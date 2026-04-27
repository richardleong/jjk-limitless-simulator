#include "Particle.h"
#include "Constants.h"
#include <random>
#include <cstdint>
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

Particle::Particle()
{
    respawn();
}

void Particle::respawn()
{
    position = { randFloat(0.f, WINDOW_WIDTH), randFloat(0.f, WINDOW_HEIGHT) };
    velocity = { randFloat(-30.f, 30.f), randFloat(-30.f, 30.f) };
    maxLifetime = randFloat(3.f, 8.f);
    lifetime = randFloat(0.f, maxLifetime);
    radius = randFloat(1.5f, 4.f);

    shape.setRadius(radius);
    shape.setOrigin({ radius, radius });

    int type = std::uniform_int_distribution<int>(0, 2)(prng());
    if (type == 0) colour = sf::Color(100, 150, 255);
    else if (type == 1) colour = sf::Color(200, 200, 255);
    else colour = sf::Color(180, 100, 255);
}

void Particle::update(float dt)
{
    lifetime -= dt;
    if (lifetime <= 0.f)
    {
        respawn();
        return;
    }
    position += velocity * dt;

    // cap max speed
    float maxSpeed = 600.f;
    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (speed > maxSpeed)
    {
        velocity.x = (velocity.x / speed) * maxSpeed;
        velocity.y = (velocity.y / speed) * maxSpeed;
    }

    // respawn if too far off screen
    float margin = 200.f;
    if (position.x < -margin || position.x > WINDOW_WIDTH + margin ||
        position.y < -margin || position.y > WINDOW_HEIGHT + margin)
    {
        respawn();
    }
}

void Particle::draw(sf::RenderWindow& window)
{
    float alpha = (lifetime / maxLifetime) * 255.f;
    sf::Color drawColour = beingPulled ? sf::Color(100, 180, 255) : colour;
    shape.setPosition(position);
    shape.setFillColor(sf::Color(drawColour.r, drawColour.g, drawColour.b, (uint8_t)alpha));
    window.draw(shape, sf::BlendAdd);
}