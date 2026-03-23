#include "Game/Projectile/Behavior.hpp"
#include "Game/Camera.hpp"
#include "Game/Projectile/Projectile.hpp"
#include "config.hpp"
#include <cmath>
#include <random>

namespace Game::Projectile {

static std::mt19937 &RNG() {
    static std::mt19937 gen(std::random_device{}());
    return gen;
}

// ── StaticFieldAoE ──────────────────────────────────────

void StaticFieldAoE::Start(Projectile &p, const BehaviorContext &ctx) {
    // Offset is set by the weapon before Start() is called
    p.SetPosition(ctx.playerPos + offset);
}

void StaticFieldAoE::Update(Projectile &p, float dt,
                            const glm::vec2 &playerPos) {
    p.SetPosition(playerPos + offset);
}

void StaticFieldAoE::Reset() {
    offset = {0, 0};
}

// ── DirectLine ──────────────────────────────────────────

void DirectLine::Start(Projectile &p, const BehaviorContext &ctx) {
    speed = p.Get("speed");
    if (ctx.hasNearestEnemy) {
        glm::vec2 diff = ctx.nearestEnemy - ctx.playerPos;
        float len = glm::length(diff);
        direction = (len > 0.001f) ? diff / len : glm::vec2{1, 0};
    } else {
        direction = ctx.cursorDir;
    }
    p.SetPosition(ctx.playerPos);
}

void DirectLine::Update(Projectile &p, float dt, const glm::vec2 &playerPos) {
    p.SetPosition(p.GetPosition() + direction * speed * dt);
}

void DirectLine::Reset() {
    direction = {1, 0};
    speed = 0;
}

// ── CursorDirection ─────────────────────────────────────

void CursorDirection::Start(Projectile &p, const BehaviorContext &ctx) {
    speed = p.Get("speed");
    direction = ctx.cursorDir;
    float len = glm::length(direction);
    if (len > 0.001f) {
        direction /= len;
    } else {
        direction = {1, 0};
    }
    p.SetPosition(ctx.playerPos);
}

void CursorDirection::Update(Projectile &p, float dt,
                             const glm::vec2 &playerPos) {
    p.SetPosition(p.GetPosition() + direction * speed * dt);
}

void CursorDirection::Reset() {
    direction = {1, 0};
    speed = 0;
}

// ── ParabolicArc ────────────────────────────────────────

void ParabolicArc::Start(Projectile &p, const BehaviorContext &ctx) {
    speed = p.Get("speed");
    origin = ctx.playerPos;
    elapsed = 0;
    pastApex = false;

    // Random angle between 45° and 135° (upward arc)
    std::uniform_real_distribution<float> dist(0.785f, 2.356f); // π/4 to 3π/4
    angle = dist(RNG());

    p.SetPosition(origin);
}

void ParabolicArc::Update(Projectile &p, float dt, const glm::vec2 &playerPos) {
    elapsed += dt;
    float x = origin.x + speed * elapsed * std::cos(angle);
    float y = origin.y - speed * elapsed * std::sin(angle) +
              0.5f * gravity * elapsed * elapsed;
    float prevY = p.GetPosition().y;
    p.SetPosition({x, y});

    if (!pastApex && y > prevY) {
        pastApex = true;
    }
}

void ParabolicArc::Reset() {
    origin = {0, 0};
    angle = 0;
    speed = 0;
    elapsed = 0;
    pastApex = false;
}

// ── BoomerangArc ────────────────────────────────────────

void BoomerangArc::Start(Projectile &p, const BehaviorContext &ctx) {
    speed = p.Get("speed");
    origin = ctx.playerPos;
    elapsed = 0;
    pastApex = false;

    if (ctx.hasNearestEnemy) {
        glm::vec2 diff = ctx.nearestEnemy - ctx.playerPos;
        float len = glm::length(diff);
        targetDir = (len > 0.001f) ? diff / len : glm::vec2{1, 0};
    } else {
        targetDir = ctx.cursorDir;
    }
    // Compute launch angle from target direction
    angle = std::atan2(-targetDir.y, targetDir.x);
    p.SetPosition(origin);
}

void BoomerangArc::Update(Projectile &p, float dt, const glm::vec2 &playerPos) {
    elapsed += dt;
    // Parabolic in local coords, rotated toward target
    float localX = speed * elapsed;
    float localY = -speed * elapsed * 0.5f + 0.5f * gravity * elapsed * elapsed;

    // Rotate by target direction angle
    float cosA = targetDir.x;
    float sinA = targetDir.y;
    float worldX = origin.x + localX * cosA - localY * sinA;
    float worldY = origin.y + localX * sinA + localY * cosA;

    float prevY = p.GetPosition().y;
    p.SetPosition({worldX, worldY});

    if (!pastApex && worldY > prevY && elapsed > 0.1f) {
        pastApex = true;
    }
}

void BoomerangArc::Reset() {
    origin = {0, 0};
    targetDir = {1, 0};
    angle = 0;
    speed = 0;
    elapsed = 0;
    pastApex = false;
}

// ── OrbitalCircle ───────────────────────────────────────

void OrbitalCircle::Start(Projectile &p, const BehaviorContext &ctx) {
    float speed_stat = p.Get("speed");
    radius = p.Get("area") * 50.0f; // area scales orbit radius
    angularSpeed = speed_stat / radius / 20.0f;

    // Distribute projectiles evenly around the circle
    if (ctx.totalAmount > 0) {
        currentAngle = (2.0f * M_PI * ctx.projectileIndex) /
                       static_cast<float>(ctx.totalAmount);
    }

    float x = ctx.playerPos.x + radius * std::cos(currentAngle);
    float y = ctx.playerPos.y + radius * std::sin(currentAngle);
    p.SetPosition({x, y});
}

void OrbitalCircle::Update(Projectile &p, float dt,
                           const glm::vec2 &playerPos) {
    currentAngle += angularSpeed * dt;
    float x = playerPos.x + radius * std::cos(currentAngle);
    float y = playerPos.y + radius * std::sin(currentAngle);
    p.SetPosition({x, y});
}

void OrbitalCircle::Reset() {
    radius = 100.0f;
    currentAngle = 0;
    angularSpeed = 0;
    permanent = false;
}

// ── ConeSpread ──────────────────────────────────────────

void ConeSpread::Start(Projectile &p, const BehaviorContext &ctx) {
    speed = p.Get("speed");

    // Base direction: toward nearest enemy or cursor
    glm::vec2 baseDir = ctx.cursorDir;
    if (ctx.hasNearestEnemy) {
        glm::vec2 diff = ctx.nearestEnemy - ctx.playerPos;
        float len = glm::length(diff);
        if (len > 0.001f)
            baseDir = diff / len;
    }

    // Spread within ±36° cone (0.628 rad), alternating sides
    float baseAngle = std::atan2(baseDir.y, baseDir.x);
    int idx = ctx.projectileIndex;
    float da = 0.628f / static_cast<float>(std::max(ctx.totalAmount, 1));
    float offset_angle =
        da * ((idx & 1) ? ((idx + 1) >> 1) : -((idx + 1) >> 1));
    float finalAngle = baseAngle + offset_angle;

    direction = {std::cos(finalAngle), std::sin(finalAngle)};
    p.SetPosition(ctx.playerPos);
}

void ConeSpread::Update(Projectile &p, float dt, const glm::vec2 &playerPos) {
    p.SetPosition(p.GetPosition() + direction * speed * dt);
}

void ConeSpread::Reset() {
    direction = {1, 0};
    speed = 0;
}

// ── BouncingWall ────────────────────────────────────────

void BouncingWall::Start(Projectile &p, const BehaviorContext &ctx) {
    speed = p.Get("speed");
    // Random direction
    std::uniform_real_distribution<float> dist(0.0f, 2.0f * M_PI);
    float angle = dist(RNG());
    direction = {std::cos(angle), std::sin(angle)};
    p.SetPosition(ctx.playerPos);
}

void BouncingWall::Update(Projectile &p, float dt, const glm::vec2 &playerPos) {
    glm::vec2 pos = p.GetPosition() + direction * speed * dt;

    // Bounce off viewport edges (player-centered viewport)
    float halfW = PTSD_Config::WINDOW_WIDTH * 0.5f;
    float halfH = PTSD_Config::WINDOW_HEIGHT * 0.5f;
    float left = playerPos.x - halfW;
    float right = playerPos.x + halfW;
    float top = playerPos.y - halfH;
    float bottom = playerPos.y + halfH;

    if (pos.x < left || pos.x > right) {
        direction.x = -direction.x;
        pos.x = std::clamp(pos.x, left, right);
    }
    if (pos.y < top || pos.y > bottom) {
        direction.y = -direction.y;
        pos.y = std::clamp(pos.y, top, bottom);
    }
    p.SetPosition(pos);
}

void BouncingWall::Reset() {
    direction = {1, 0};
    speed = 0;
}

// ── SeekingExplosion ────────────────────────────────────

void SeekingExplosion::Start(Projectile &p, const BehaviorContext &ctx) {
    speed = p.Get("speed");
    exploded = false;

    if (ctx.hasNearestEnemy) {
        target = ctx.nearestEnemy;
    } else {
        // Random position near player
        std::uniform_real_distribution<float> dist(-150.0f, 150.0f);
        target = ctx.playerPos + glm::vec2{dist(RNG()), dist(RNG())};
    }
    glm::vec2 diff = target - ctx.playerPos;
    float len = glm::length(diff);
    direction = (len > 0.001f) ? diff / len : glm::vec2{0, -1};
    p.SetPosition(ctx.playerPos);
}

void SeekingExplosion::Update(Projectile &p, float dt,
                              const glm::vec2 &playerPos) {
    if (!exploded) {
        glm::vec2 pos = p.GetPosition();
        float dist = glm::distance(pos, target);
        if (dist < p.Get("area") * 50.0f) {
            // Arrived — trigger explosion phase
            exploded = true;
            speed /= 10.0f;
        } else {
            p.SetPosition(pos + direction * speed * dt);
        }
    } else {
        // Explosion phase: grow AoE, slow down
        p.Set("area", p.Get("area") + 0.5f * dt);
    }
}

void SeekingExplosion::Reset() {
    target = {0, 0};
    direction = {0, 0};
    speed = 0;
    exploded = false;
}

// ── Unimplemented ───────────────────────────────────────

void Unimplemented::Start(Projectile &, const BehaviorContext &ctx) {
    // No movement for unimplemented weapons
}

void Unimplemented::Update(Projectile &, float, const glm::vec2 &) {
    // Do nothing
}

void Unimplemented::Reset() {}

} // namespace Game::Projectile
