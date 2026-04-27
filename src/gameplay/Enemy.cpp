#include "Enemy.h"
#include "core/Constants.h"
#include "core/Utils.h"
#include <cmath>

Enemy::Enemy(EnemyType t, sf::Vector2f startPos)
    : type(t), position(startPos)
{
    alive = true;
    velocity = { randFloat(-20.f, 20.f), randFloat(-20.f, 20.f) };

    switch (type) {
    case EnemyType::Weak:
        collisionRadius = 10.f;
        health = maxHealth = 30.f;
        break;
    case EnemyType::Basic:
        collisionRadius = 14.f;
        health = maxHealth = 60.f;
        break;
    case EnemyType::Anchor:
        collisionRadius = 20.f;
        health = maxHealth = 150.f;
        break;
    case EnemyType::Blink:
        collisionRadius = 12.f;
        health = maxHealth = 50.f;
        dashTimer = randFloat(0.f, dashCooldown);
        break;
    }

    setupVisual();
}

void Enemy::setupVisual() {
    float barWidth = collisionRadius * 2.f;

    // Health bar background
    healthBarBackground.setSize({ barWidth, 4.f });
    healthBarBackground.setFillColor(sf::Color(40, 40, 40, 200));

    // Health bar fill
    healthBarFill.setSize({ barWidth, 4.f });
    healthBarFill.setFillColor(sf::Color(255, 60, 60, 220));

    // Shape
    shape.setRadius(collisionRadius);
    shape.setOrigin({ collisionRadius, collisionRadius });

    switch (type) {
    case EnemyType::Weak:
        shape.setPointCount(6);
        shape.setFillColor(sf::Color(120, 30, 30, 230));
        break;
    case EnemyType::Basic:
        shape.setPointCount(5);
        shape.setFillColor(sf::Color(160, 40, 40, 240));
        break;
    case EnemyType::Anchor:
        shape.setPointCount(8);
        shape.setFillColor(sf::Color(80, 20, 80, 240));
        shape.setOutlineThickness(2.f);
        shape.setOutlineColor(sf::Color(200, 100, 200, 150));
        break;
    case EnemyType::Blink:
        shape.setPointCount(4);
        shape.setFillColor(sf::Color(200, 200, 100, 200));
        break;
    }
}

void Enemy::update(float dt, sf::Vector2f gojoPos) {
    if (!alive) return;

    // stun prevents action
    if (stunTimer > 0.f) {
        stunTimer -= dt;
        position += velocity * dt;
        position.x = std::clamp(position.x, collisionRadius, WINDOW_WIDTH - collisionRadius);
        position.y = std::clamp(position.y, collisionRadius, WINDOW_HEIGHT - collisionRadius);
        shape.setPosition(position);
        updateHealthBar();
        return;
    }

    // per‑type movement
    updateMovement(dt, gojoPos);

    // clamp to screen (except Anchor, they bounce)
    if (type != EnemyType::Anchor) {
        position.x = std::clamp(position.x, collisionRadius, WINDOW_WIDTH - collisionRadius);
        position.y = std::clamp(position.y, collisionRadius, WINDOW_HEIGHT - collisionRadius);
    }
    else {
        // Anchor bounces off edges
        if (position.x < collisionRadius || position.x > WINDOW_WIDTH - collisionRadius) {
            velocity.x = -velocity.x;
            position.x = std::clamp(position.x, collisionRadius, WINDOW_WIDTH - collisionRadius);
        }
        if (position.y < collisionRadius || position.y > WINDOW_HEIGHT - collisionRadius) {
            velocity.y = -velocity.y;
            position.y = std::clamp(position.y, collisionRadius, WINDOW_HEIGHT - collisionRadius);
        }
    }

    shape.setPosition(position);
    updateHealthBar();
}

void Enemy::updateMovement(float dt, sf::Vector2f gojoPos) {
    switch (type) {
    case EnemyType::Weak: {
        velocity.x += randFloat(-80.f, 80.f) * dt;
        velocity.y += randFloat(-80.f, 80.f) * dt;
        velocity *= 0.98f;
        position += velocity * dt;
        break;
    }
    case EnemyType::Basic: {
        sf::Vector2f toGojo = gojoPos - position;
        float dist = std::sqrt(toGojo.x * toGojo.x + toGojo.y * toGojo.y);
        if (dist > 1.f) {
            sf::Vector2f desired = (toGojo / dist) * 40.f;
            sf::Vector2f steer = desired - velocity;
            float steerLen = std::sqrt(steer.x * steer.x + steer.y * steer.y);
            if (steerLen > 100.f) steer = (steer / steerLen) * 100.f;
            velocity += steer * dt;
        }
        velocity *= 0.95f;
        position += velocity * dt;
        break;
    }
    case EnemyType::Anchor: {
        sf::Vector2f centre(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
        sf::Vector2f toCentre = centre - position;
        sf::Vector2f tangent = { -toCentre.y, toCentre.x };
        float len = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
        if (len > 1.f) tangent = tangent / len * 30.f;
        velocity += (tangent - velocity) * 0.5f * dt;
        position += velocity * dt;
        break;
    }
    case EnemyType::Blink: {
        if (isDashing) {
            dashTimerDuration -= dt;
            velocity = dashDirection * dashSpeed;
            position += velocity * dt;
            if (dashTimerDuration <= 0.f) {
                isDashing = false;
                velocity *= 0.3f;
                dashTimer = dashCooldown;
            }
        }
        else {
            dashTimer -= dt;
            if (dashTimer <= 0.f) {
                isDashing = true;
                dashTimerDuration = dashDuration;
                sf::Vector2f toGojo = gojoPos - position;
                float dist = std::sqrt(toGojo.x * toGojo.x + toGojo.y * toGojo.y);
                if (dist > 1.f) {
                    dashDirection = toGojo / dist;
                }
                else {
                    dashDirection = { randFloat(-1.f, 1.f), randFloat(-1.f, 1.f) };
                }
            }
            sf::Vector2f toGojo = gojoPos - position;
            float dist = std::sqrt(toGojo.x * toGojo.x + toGojo.y * toGojo.y);
            if (dist > 1.f) {
                sf::Vector2f desired = (toGojo / dist) * 50.f;
                velocity += (desired - velocity) * 1.5f * dt;
            }
            position += velocity * dt;
        }
        break;
    }
    }
}

void Enemy::draw(sf::RenderWindow& window) {
    if (!alive) return;
    if (type == EnemyType::Blink) {
        float alpha = 150.f + std::sin(inBlueTimer * 20.f) * 50.f;
        sf::Color col = shape.getFillColor();
        col.a = static_cast<uint8_t>(alpha);
        shape.setFillColor(col);
    }
    window.draw(shape);
    window.draw(healthBarBackground);
    window.draw(healthBarFill);
}

void Enemy::takeDamage(float amount) {
    health -= amount;
    updateHealthBar();
    if (health <= 0.f) {
        alive = false;
    }
}

void Enemy::updateHealthBar() {
    float barWidth = collisionRadius * 2.f;
    float leftEdge = position.x - collisionRadius;
    float barY = position.y - collisionRadius - 8.f;

    healthBarBackground.setPosition({ leftEdge, barY });
    healthBarBackground.setSize({ barWidth, 4.f });

    float healthPercent = health / maxHealth;
    float newWidth = barWidth * healthPercent;
    healthBarFill.setPosition({ leftEdge, barY });
    healthBarFill.setSize({ newWidth, 4.f });
}