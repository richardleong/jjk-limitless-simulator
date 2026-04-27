#pragma once
#include <random>
#include <SFML/System/Vector2.hpp>

std::mt19937& prng();
float randFloat(float min, float max);

void applyAttractionForce(sf::Vector2f& velocity, sf::Vector2f position,
    sf::Vector2f target, float strength, float dt);
void applyRepulsionForce(sf::Vector2f& velocity, sf::Vector2f position,
    sf::Vector2f origin, float strength, float radius, float dt);
bool isInsideRadius(sf::Vector2f position, sf::Vector2f centre, float radius);