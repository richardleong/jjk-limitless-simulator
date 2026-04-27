// ScreenShake.h
#pragma once
#include <SFML/System/Vector2.hpp>

class ScreenShake {
    float m_intensity = 0.f;
public:
    void addShake(float intensity);
    void update(float dt);
    sf::Vector2f getOffset() const;
private:
    sf::Vector2f m_offset;
};