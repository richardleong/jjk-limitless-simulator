#pragma once
#include <SFML/Graphics.hpp>
#include <cstdint>

enum class EnemyType {
    Weak,    // low‑level mob, obliterated by Blue after 2s
    Basic,   // drifts toward Gojo, damaged at Blue centre only
    Anchor,  // resists Blue, heavy knockback but durable
    Blink    // teleports, disrupted by Blue/Red
};

class Enemy {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    bool alive = true;
    float collisionRadius = 12.f;
    float health = 100.f;
    float maxHealth = 100.f;

    // behaviour state
    float inBlueTimer = 0.f;       // time spent inside Blue's kill radius
    float stunTimer = 0.f;         // time left unable to act after Red hit
    float dashCooldown = 2.f;      // time between dashes
    float dashTimer = 0.f;         // countdown to next dash
    bool isDashing = false;        // currently in dash burst
    float dashDuration = 0.15f;    // how long the burst lasts
    float dashTimerDuration = 0.f; // countdown for dash burst
    sf::Vector2f dashDirection;    // direction of dash
    float dashSpeed = 400.f;       // speed during dash

    sf::CircleShape shape;
    sf::RectangleShape healthBarBackground;
    sf::RectangleShape healthBarFill;

    EnemyType type;

    Enemy(EnemyType t, sf::Vector2f startPos);
    void update(float dt, sf::Vector2f gojoPos);
    void draw(sf::RenderWindow& window);
    void takeDamage(float amount);
private:
    void updateMovement(float dt, sf::Vector2f gojoPos);
    void setupVisual();
    void updateHealthBar();
};