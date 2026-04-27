#include "Constants.h"
#include "Particle.h"
#include <random>
#include <cstdint>

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
    lifetime = randFloat(0.f, maxLifetime); // stagger so not all fade at once
    radius = randFloat(1.5f, 4.f);

    shape.setRadius(radius);
    shape.setOrigin({ radius, radius });

    // mostly blue/white debris — cursed energy aesthetic
    int type = std::uniform_int_distribution<int>(0, 2)(prng());
    if (type == 0) colour = sf::Color(100, 150, 255); // blue
    else if (type == 1) colour = sf::Color(200, 200, 255); // white-blue
    else colour = sf::Color(180, 100, 255); // purple
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

    // wrap around screen edges
    if (position.x < 0) position.x = WINDOW_WIDTH;
    if (position.x > WINDOW_WIDTH) position.x = 0.f;
    if (position.y < 0) position.y = WINDOW_HEIGHT;
    if (position.y > WINDOW_HEIGHT) position.y = 0.f;
}

void Particle::draw(sf::RenderWindow& window)
{
    float alpha = (lifetime / maxLifetime) * 255.f;
    shape.setPosition(position);
    shape.setFillColor(sf::Color(colour.r, colour.g, colour.b, (uint8_t)alpha));
    window.draw(shape, sf::BlendAdd); // additive blending for glow
}