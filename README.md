# JJK Limitless Simulator

A real-time physics simulation of Satoru Gojo's **Limitless** cursed techniques from Jujutsu Kaisen, built in **C++17** with **SFML 3.1.0**. Each ability is grounded in real physics — inverse-square force fields, impulse-based collision resolution, and exponential falloff curves.

> This is Project 2 of a self-directed C++ learning portfolio, built to demonstrate real-time simulation, game architecture, and physics programming.

---

## Abilities

### Blue — Cursed Technique Lapse
Hold `Q` or `Left Click` to activate. Creates a gravitational singularity at the cursor — all particles and enemies are pulled inward using an inverse-square attraction force.

```
F = strength / (distance² + ε)
```

The epsilon term prevents force singularities at zero distance, producing smooth and stable behaviour even at close range.

### Red — Cursed Technique Reversal
Press `E` or `Right Click` to activate. Charges visibly before detonating — Red orb grows during charge, then releases an explosive shockwave with exponential radial falloff. The expanding shockwave ring is a functional physics collider, pushing particles outward as it passes through them.

```
F = strength × e^(−distance × falloff)
```

Different enemy types respond differently: Anchor resists knockback, Blink gets stunned, Weak enemies are instantly destroyed.

### Hollow Purple
Hold `Space` or `Middle Click` to activate. A six-stage cinematic sequence:

1. **Blue orb** materialises and orbits
2. **Red orb** appears on the opposite side, both orbit the merge point
3. **Collision phase** — orbs spiral inward, shockwave rings pulse at the collision point
4. **Screen flash** — purple overlay floods the screen
5. **Pause** — Purple sphere holds position, pulsating with chaotic energy
6. **Travel** — sphere fires toward the locked aim point, erasing everything in its path

5-second cooldown. Cancels with no cooldown if released before the collision phase.

---

## Enemies

| Type | Shape | Behaviour |
|---|---|---|
| Weak | Hexagon | Jitters randomly, dies easily to all abilities |
| Basic | Pentagon | Drifts toward Gojo, requires sustained Blue or direct Red hit |
| Anchor | Octagon | Circles the map, heavily resists Blue and Red knockback |
| Blink | Diamond | Dashes toward Gojo in bursts, disrupted and stunned by Red |

All enemies have health bars and differentiated responses to each ability.

---

## Architecture

```
src/
├── main.cpp                      # window, input, game loop
├── core/
│   ├── Constants.h               # window dimensions, tuning values
│   └── Utils.h / .cpp            # randFloat, force helper functions
├── gameplay/
│   ├── Particle.h / .cpp         # individual particle with lifetime and glow
│   ├── ParticleSystem.h / .cpp   # manages 300 particles, exposes force API
│   ├── Enemy.h / .cpp            # enemy entity with per-type AI and visuals
│   └── EnemyManager.h / .cpp     # spawning, updating, ability interaction hooks
├── abilities/
│   ├── Ability.h                 # abstract base class
│   ├── BlueAbility.h / .cpp      # attraction field
│   ├── RedAbility.h / .cpp       # repulsion shockwave
│   └── PurpleAbility.h / .cpp    # 6-stage cinematic ability with state machine
└── systems/
    ├── GameWorld.h / .cpp        # orchestration layer — owns all subsystems
    └── ScreenShake.h / .cpp      # camera offset with intensity decay
```

Abilities communicate with the world through `GameWorld` — they never access `ParticleSystem` or `EnemyManager` directly. This keeps the ability layer decoupled from the simulation layer.

---

## Physics Reference

| System | Formula |
|---|---|
| Blue attraction | `F = strength / (d² + ε)` |
| Red repulsion | `F = strength × exp(−d × falloff)` |
| Expanding shockwave | particles in ring front `[prev_r, curr_r]` per frame |
| Screen shake | `offset = rand(±intensity)`, `intensity *= 0.9` per frame |
| Additive glow | `window.draw(shape, sf::BlendAdd)` |
| Particle fade | `alpha = (lifetime / maxLifetime) × 255` |

---

## Building

SFML is included in the repository under `SFML/` — no separate installation required.

```bash
git clone https://github.com/richardleong/jjk-limitless-simulator.git
cd jjk-limitless-simulator
mkdir build && cd build
cmake ..
cmake --build .
./GojoLimitlessSimulator
```

> Requires CMake 3.22+ and a C++17 compatible compiler. MSVC recommended on Windows.

---

## Controls

| Input | Action |
|---|---|
| `WASD` | Move Gojo |
| `Q` / Left Click | Blue — hold to attract |
| `E` / Right Click | Red — press to charge and detonate |
| `Space` / Middle Click | Purple — hold to begin sequence |
| Mouse | Aim point for all abilities |

---

## Known Improvements

- Sprite integration — replace placeholder shapes with Gojo and cursed spirit sprites
- Infinity passive — debris approaching Gojo slows exponentially (Zeno's paradox)
- Wave system — timed enemy spawning rather than fixed initial wave
- Anchor fragmentation — Purple hitting Anchor splits it into 3 Weak enemies
- AbilitySystem manager — centralise ability registration and cooldown UI
