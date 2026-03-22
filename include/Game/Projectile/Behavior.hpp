#ifndef PROJECTILE_BEHAVIOR_HPP
#define PROJECTILE_BEHAVIOR_HPP

#include "pch.hpp"
#include <variant>

namespace Game::Projectile {

class Projectile; // forward declare

// Context passed to behaviors on Start() so they can resolve targets
struct BehaviorContext {
    glm::vec2 playerPos{0, 0};
    glm::vec2 cursorDir{1, 0}; // normalized direction from player to cursor
    glm::vec2 nearestEnemy{0, 0};
    bool hasNearestEnemy = false;
    int projectileIndex = 0; // which projectile in the burst (0, 1, 2...)
    int totalAmount = 1;     // total projectiles in this burst
};

// ─────────────────────────────────────────────────────────
// 1. StaticFieldAoE — follows player at fixed offset
//    Used by: WHIP, VAMPIRICA, GARLIC, VORTEX
// ─────────────────────────────────────────────────────────
struct StaticFieldAoE {
    glm::vec2 offset{0, 0};

    void Start(Projectile &p, const BehaviorContext &ctx);
    void Update(Projectile &p, float dt, const glm::vec2 &playerPos);
    void Reset();
};

// ─────────────────────────────────────────────────────────
// 2. DirectLine — targets nearest enemy, flies straight
//    Used by: MAGIC_MISSILE, HOLY_MISSILE
// ─────────────────────────────────────────────────────────
struct DirectLine {
    glm::vec2 direction{1, 0};
    float speed = 0;

    void Start(Projectile &p, const BehaviorContext &ctx);
    void Update(Projectile &p, float dt, const glm::vec2 &playerPos);
    void Reset();
};

// ─────────────────────────────────────────────────────────
// 3. CursorDirection — fires toward cursor/facing direction
//    Used by: KNIFE, THOUSAND
// ─────────────────────────────────────────────────────────
struct CursorDirection {
    glm::vec2 direction{1, 0};
    float speed = 0;

    void Start(Projectile &p, const BehaviorContext &ctx);
    void Update(Projectile &p, float dt, const glm::vec2 &playerPos);
    void Reset();
};

// ─────────────────────────────────────────────────────────
// 4. ParabolicArc — gravity physics (upward launch + fall)
//    Used by: AXE, SCYTHE
// ─────────────────────────────────────────────────────────
struct ParabolicArc {
    glm::vec2 origin{0, 0};
    float angle = 0;   // launch angle (radians)
    float speed = 0;
    float elapsed = 0;
    float gravity = 980.0f;
    bool pastApex = false;

    void Start(Projectile &p, const BehaviorContext &ctx);
    void Update(Projectile &p, float dt, const glm::vec2 &playerPos);
    void Reset();
};

// ─────────────────────────────────────────────────────────
// 5. BoomerangArc — parabolic with return toward origin
//    Used by: CROSS, HEAVENSWORD
// ─────────────────────────────────────────────────────────
struct BoomerangArc {
    glm::vec2 origin{0, 0};
    glm::vec2 targetDir{1, 0};
    float angle = 0;
    float speed = 0;
    float elapsed = 0;
    float gravity = 980.0f;
    bool pastApex = false;

    void Start(Projectile &p, const BehaviorContext &ctx);
    void Update(Projectile &p, float dt, const glm::vec2 &playerPos);
    void Reset();
};

// ─────────────────────────────────────────────────────────
// 6. OrbitalCircle — orbits around player at fixed radius
//    Used by: HOLYBOOK, VESPERS
// ─────────────────────────────────────────────────────────
struct OrbitalCircle {
    float radius = 100.0f;
    float currentAngle = 0;
    float angularSpeed = 0;
    bool permanent = false; // VESPERS never expires

    void Start(Projectile &p, const BehaviorContext &ctx);
    void Update(Projectile &p, float dt, const glm::vec2 &playerPos);
    void Reset();
};

// ─────────────────────────────────────────────────────────
// 7. ConeSpread — fires within ±cone angle, straight line
//    Used by: FIREBALL, HELLFIRE
// ─────────────────────────────────────────────────────────
struct ConeSpread {
    glm::vec2 direction{1, 0};
    float speed = 0;

    void Start(Projectile &p, const BehaviorContext &ctx);
    void Update(Projectile &p, float dt, const glm::vec2 &playerPos);
    void Reset();
};

// ─────────────────────────────────────────────────────────
// 8. BouncingWall — bounces off screen edges
//    Used by: DIAMOND, ROCHER
// ─────────────────────────────────────────────────────────
struct BouncingWall {
    glm::vec2 direction{1, 0};
    float speed = 0;

    void Start(Projectile &p, const BehaviorContext &ctx);
    void Update(Projectile &p, float dt, const glm::vec2 &playerPos);
    void Reset();
};

// ─────────────────────────────────────────────────────────
// 9. SeekingExplosion — homes toward target, then AoE grows
//    Used by: HOLYWATER, BORA
// ─────────────────────────────────────────────────────────
struct SeekingExplosion {
    glm::vec2 target{0, 0};
    glm::vec2 direction{0, 0};
    float speed = 0;
    bool exploded = false;

    void Start(Projectile &p, const BehaviorContext &ctx);
    void Update(Projectile &p, float dt, const glm::vec2 &playerPos);
    void Reset();
};

// ─────────────────────────────────────────────────────────
// 10. Unimplemented — placeholder for weapons with no
//     movement code in OOPL-VS (37 of 57 weapons)
// ─────────────────────────────────────────────────────────
struct Unimplemented {
    void Start(Projectile &p, const BehaviorContext &ctx);
    void Update(Projectile &p, float dt, const glm::vec2 &playerPos);
    void Reset();
};

// The variant type — no heap allocation, pool-friendly
using BehaviorVariant = std::variant<
    StaticFieldAoE,
    DirectLine,
    CursorDirection,
    ParabolicArc,
    BoomerangArc,
    OrbitalCircle,
    ConeSpread,
    BouncingWall,
    SeekingExplosion,
    Unimplemented>;

} // namespace Game::Projectile

#endif // PROJECTILE_BEHAVIOR_HPP
