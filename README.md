================================================================
PROJECT 2 — GOJO'S LIMITLESS TECHNIQUES
Real-Time Simulation | C++ + SFML 3.1.0
================================================================

LAYER 1 — World + Gojo Setup
------------------------------
[ ] Dark background with subtle grid (void aesthetic)
[ ] Gojo as white glowing circle placeholder
[ ] WASD movement with delta time
[ ] Mouse cursor visible as crosshair/aim point
[ ] Window 1280x720

LAYER 2 — Particle System
---------------------------
[ ] Particle class: position, velocity, colour, lifetime, size
[ ] Spawn 300 debris particles at random positions
[ ] Particles drift slowly with slight random velocity
[ ] Particles fade out when lifetime expires, respawn
[ ] Draw with additive blending for glow effect

LAYER 3 — Blue (Attraction)
-----------------------------
[ ] Hold Q to activate Blue
[ ] While held: apply attraction force to all particles toward mouse cursor
[ ] Force formula: F = strength / distance² (like gravity)
[ ] Force capped at max value so particles don't teleport
[ ] Visual: blue pulsing ring at cursor while held
[ ] Visual: particles tint blue when pulled
[ ] Particles that reach cursor orbit it briefly then get destroyed

LAYER 4 — Red (Repulsion)
---------------------------
[ ] Press E to activate Red (instant shockwave, not held)
[ ] Apply explosive outward force from mouse cursor
[ ] Force formula: F = strength / distance² but outward
[ ] Particles near cursor get obliterated (fade out instantly)
[ ] Visual: red expanding ring from cursor on activation
[ ] Visual: screen shake proportional to nearby particles hit
[ ] Particles that hit window edge bounce or disappear

LAYER 5 — Hollow Purple (Beam)
--------------------------------
[ ] Press Space to fire Hollow Purple
[ ] Beam fires horizontally from Gojo position across screen
[ ] Beam has width — anything within beam height gets erased
[ ] Erasure: particles touched by beam explode into smaller fragments
[ ] Visual: thick purple beam with additive glow
[ ] Visual: beam appears for 0.5s then fades
[ ] Visual: heavy screen shake on activation
[ ] Cooldown: 3 seconds between uses

LAYER 6 — Enemies
-------------------
[ ] Enemy class: position, size, colour (cursed spirit aesthetic)
[ ] Spawn 10-15 enemies at random positions
[ ] Enemies float with very slight random drift (not chasing)
[ ] Enemies react to abilities:
    - Blue: pulled toward cursor like particles
    - Red: thrown outward, destroyed if hitting wall
    - Purple: instantly erased if in beam path
[ ] Visual: enemies glow red/purple, distinct from particles
[ ] On death: burst into particles (reuse particle system)

LAYER 7 — Glow + Visual Polish
---------------------------------
[ ] Additive blending on all ability VFX
[ ] Gojo glows white — layered circles with decreasing opacity
[ ] Blue ability: blue vignette on screen edges while held
[ ] Red ability: red flash on screen for 0.1s on activation  
[ ] Purple beam: purple bloom that fills screen briefly
[ ] Particles leave motion trails (ghost previous position)

LAYER 8 — Screen Shake
------------------------
[ ] ScreenShake class: intensity, duration, decay
[ ] Camera offset applied to all draw calls
[ ] Blue: subtle shake while held
[ ] Red: strong shake on activation
[ ] Purple: violent shake for 0.5s

LAYER 9 — Infinity Passive (BONUS)
------------------------------------
[ ] Debris approaching within 80px of Gojo slows exponentially
[ ] velocity *= (distance / minDistance) each frame
[ ] Objects never fully reach Gojo — Zeno's paradox in code
[ ] Visual: faint white ring around Gojo showing infinity radius

LAYER 10 — Sprites (LAST)
---------------------------
[ ] Load Gojo sprite sheet from assets/
[ ] Animation states: idle, blue_cast, red_cast, purple_cast
[ ] Replace placeholder circle with sprite
[ ] Replace ability VFX placeholders with sprite animations
[ ] Enemies get cursed spirit sprites

================================================================
COMMIT STRATEGY
================================================================
feat: Layer 1 - world setup and Gojo WASD movement
feat: Layer 2 - particle system with additive blending
feat: Layer 3 - Blue attraction force field
feat: Layer 4 - Red repulsion shockwave
feat: Layer 5 - Hollow Purple beam and erasure
feat: Layer 6 - enemy entities and ability reactions
feat: Layer 7 - glow effects and visual polish
feat: Layer 8 - screen shake system
feat: Layer 9 - Infinity passive defence mechanic
feat: Layer 10 - sprite integration

================================================================
PHYSICS REFERENCE
================================================================
Blue attraction:  F = strength / distance²  (inward)
Red repulsion:    F = strength / distance²  (outward)
Purple erasure:   if abs(particle.y - beam.y) < beamWidth/2 → destroy
Infinity:         velocity *= (distance / minRadius) when distance < minRadius
Screen shake:     cameraOffset = random(-intensity, intensity) each frame
                  intensity *= 0.9f each frame (decay)
Glow:             window.draw(shape, sf::BlendAdd)
================================================================