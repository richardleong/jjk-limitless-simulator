#include "EnemyManager.h"
#include "core/Utils.h"
#include "core/Constants.h"
#include <cmath>

void EnemyManager::spawnEnemy(EnemyType type, sf::Vector2f pos) {
    m_enemies.emplace_back(type, pos);
}

void EnemyManager::update(float dt, sf::Vector2f gojoPos) {
    for (auto& e : m_enemies) {
        if (e.alive) e.update(dt, gojoPos);
    }
    clearDead();
}

void EnemyManager::draw(sf::RenderWindow& window) {
    for (auto& e : m_enemies) {
        if (e.alive) e.draw(window);
    }
}

void EnemyManager::clearDead() {
    m_enemies.erase(std::remove_if(m_enemies.begin(), m_enemies.end(),
        [](const Enemy& e) { return !e.alive; }),
        m_enemies.end());
}

// === Ability interaction methods ===

void EnemyManager::applyForce(sf::Vector2f force) {
    for (auto& e : m_enemies) {
        if (!e.alive) continue;
        e.velocity += force;
    }
}

void EnemyManager::applyRadialForce(sf::Vector2f origin, float strength, float radius) {
    for (auto& e : m_enemies) {
        if (!e.alive) continue;
        applyRepulsionForce(e.velocity, e.position, origin, strength, radius, 1.f);
    }
}

void EnemyManager::killInRadius(sf::Vector2f centre, float radius) {
    for (auto& e : m_enemies) {
        if (!e.alive) continue;
        if (isInsideRadius(e.position, centre, radius))
            e.alive = false;
    }
}

void EnemyManager::applyBlueEffect(sf::Vector2f centre, float innerRadius, float damageRadius, float dt) {
    // innerRadius: area where weak enemies die after 2s
    // damageRadius: area where Basic enemies take damage over time
    for (auto& e : m_enemies) {
        if (!e.alive) continue;
        float dx = e.position.x - centre.x;
        float dy = e.position.y - centre.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (e.type == EnemyType::Weak) {
            if (dist < innerRadius) {
                e.inBlueTimer += dt;
                if (e.inBlueTimer >= 2.0f) e.alive = false;
            }
            else {
                e.inBlueTimer = std::max(0.f, e.inBlueTimer - dt * 2.f); // decay when outside
            }
        }
        else if (e.type == EnemyType::Basic) {
            if (dist < innerRadius * 0.5f) {  // very small centre
                e.inBlueTimer += dt;
                if (e.inBlueTimer >= 4.0f) e.alive = false;  // longer threshold
            }
            else {
                e.inBlueTimer = std::max(0.f, e.inBlueTimer - dt * 2.f);
            }
        }
        // Anchor and Blink are unaffected by Blue's kill mechanic
    }
}

void EnemyManager::applyRedEffect(sf::Vector2f origin, float strength, float killRadius) {
    for (auto& e : m_enemies) {
        if (!e.alive) continue;
        float dx = e.position.x - origin.x;
        float dy = e.position.y - origin.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        // outward force (same as applyRadialForce but with tweaks per type)
        if (dist < killRadius && dist > 1.f) {
            sf::Vector2f dir = { dx / dist, dy / dist };
            float resistance = 1.f;
            if (e.type == EnemyType::Anchor) resistance = 0.3f;  // heavy knockback resistance
            if (e.type == EnemyType::Blink) {
                // cancel dash + stun
                e.isDashing = false;
                e.stunTimer = 0.5f;
                e.dashTimer = e.dashCooldown; // reset dash cooldown
            }
            float forceMag = strength * (1.f - dist / killRadius) * resistance;
            e.velocity += dir * forceMag;
        }

        // Instant kill for Weak inside kill radius
        if (e.type == EnemyType::Weak && dist < killRadius) {
            e.alive = false;
        }
        else if (e.type == EnemyType::Basic && dist < killRadius * 0.5f) {
            e.alive = false;  // only centre destroys Basic
        }
        else if (e.type == EnemyType::Anchor && dist < killRadius * 0.3f) {
            e.takeDamage(40.f);   // anchor takes heavy damage but not instant
        }
        else if (e.type == EnemyType::Blink) {
            // Blink takes damage based on how close they are
            float damageMult = 1.f - (dist / killRadius);  // 1.0 at centre, 0.0 at edge
            e.takeDamage(30.f * damageMult);
        }
    }
}

void EnemyManager::applyPurpleEffect(sf::Vector2f centre, float radius) {
    for (auto& e : m_enemies) {
        if (!e.alive) continue;
        float dx = e.position.x - centre.x;
        float dy = e.position.y - centre.y;
        if (dx * dx + dy * dy < radius * radius) {
            if (e.type == EnemyType::Anchor) {
                // Fragment into 3 weak enemies (optional kill for now, later spawn)
                e.alive = false;
                // future: spawn 3 Weak at e.position
            }
            else {
                e.alive = false;   // erased
            }
        }
    }
}

void EnemyManager::attractEnemies(sf::Vector2f target, float strength) {
    for (auto& e : m_enemies) {
        if (!e.alive) continue;
        float mult = 1.f;
        if (e.type == EnemyType::Anchor) mult = 0.1f;
        else if (e.type == EnemyType::Blink) mult = 0.8f;
        applyAttractionForce(e.velocity, e.position, target, strength * mult, 1.f);
    }
}