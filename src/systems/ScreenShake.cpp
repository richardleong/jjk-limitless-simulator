// ScreenShake.cpp
#include "ScreenShake.h"
#include "core/Utils.h"          // randFloat

void ScreenShake::addShake(float intensity) {
    m_intensity = std::max(m_intensity, intensity);
}

void ScreenShake::update(float dt) {
    if (m_intensity > 0.f) {
        m_offset = { randFloat(-m_intensity, m_intensity), randFloat(-m_intensity, m_intensity) };
        m_intensity -= 60.f * dt;
        if (m_intensity < 0.f) m_intensity = 0.f;
    }
    else {
        m_offset = { 0.f, 0.f };
    }
}

sf::Vector2f ScreenShake::getOffset() const {
    return m_offset;
}