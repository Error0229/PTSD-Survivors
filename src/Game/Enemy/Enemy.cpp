#include "Game/Enemy/Enemy.hpp"
#include "Game/Camera.hpp"
#include "Game/Config.hpp"
#include "Game/Resource.hpp"
#include "Game/Util/Animated.hpp"
#include "Game/Util/Timer.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "pch.hpp"
#include <cmath>

namespace Game::Enemy {
void Enemy::Start() {
    m_Type = ENEMY;
    m_DefaultDirection = Util::Direction::LEFT;
    m_["hp"] = m_["maxHp"];
    m_["isOver"] = false;
    m_["isDead"] = false;
    m_ZIndex = ENEMY_LAYER;
    Animated::Play();
}
void Enemy::Update(const ::Util::Transform &transform) {
    if (!IsDead())
        GoTo(transform.translation);
}
void Enemy::GoTo(glm::vec2 target) {
    if (target.x < m_Position.x) {
        m_Direction = Util::Direction::LEFT;
    } else {
        m_Direction = Util::Direction::RIGHT;
    }
    m_Position += m_Velocity * static_cast<float>(::Util::Time::GetDeltaTime());
    auto direction = glm::normalize(target - m_Position);
    if (m_IsStunned) {
        direction = -direction;
        if (Util::Clock.Now() - m_lastHit > 120) {
            m_IsStunned = false;
        }
    }
    m_Velocity = direction * m_["speed"];
}
void Enemy::Draw() {
    m_Transform.translation = Camera::WorldToScreen(m_Position);
    if ((IsMirrored() && m_Transform.scale.x > 0) ||
        (!IsMirrored() && m_Transform.scale.x < 0)) {
        m_Transform.scale.x *= -1;
    }
    Animated::Draw(m_Transform, m_ZIndex);
    if (IsDead() && !IsAnimated()) {
        m_["isOver"] = true;
    }
    if (m_HitVFX != nullptr) {
        m_HitVFX->Draw(m_Transform, VFX_LAYER);
        if (!m_HitVFX->IsAnimated()) {
            m_HitVFX = nullptr;
        }
    }
}
void Enemy::Hurt(float_t damage) {
    m_["hp"] -= damage;
    if (m_["hp"] <= 0 && !IsDead()) {
        m_["hp"] = 0;
        SetAnimation("Death");
        m_["isDead"] = true;
        Animated::Play();
    }
}
bool Enemy::HitBy(std::shared_ptr<Projectile::Projectile> proj,
                  float_t damage) {
    if (m_lastHitBy.find(proj) != m_lastHitBy.end() &&
        proj->Get("hitBoxDelay") > Util::Clock.Now() - m_lastHitBy.at(proj)) {
        return false;
    }
    m_lastHitBy[proj] = Util::Clock.Now();
    Hurt(damage);
    m_IsStunned = true;
    m_lastHit = Util::Clock.Now();
    m_HitVFX = Resource::GetAnimation(
        "VFX" + std::to_string(static_cast<int32_t>(proj->Get("hitVFX"))));
    m_HitVFX->Play();
    return true;
}
std::string Enemy::ID() {
    return m_ID;
}
bool Enemy::IsOver() {
    return m_["isOver"];
}

bool Enemy::IsDead() {
    return m_["isDead"];
}
bool Enemy::IsBoss() {
    return m_["isBoss"];
}
bool Enemy::IsSwarm() {
    return m_["isSwarm"];
}
void Enemy::CollisionWith(const std::shared_ptr<Enemy> &other) {
    auto overlap_x =
        std::min(m_Position.x + Width() - other->m_Position.x,
                 other->m_Position.x + other->Width() - m_Position.x);
    auto overlap_y =
        std::min(m_Position.y + Height() - other->m_Position.y,
                 other->m_Position.y + other->Height() - m_Position.y);
    glm::vec2 normal{0, 0};
    static float_t percent = 0.2f;
    static float_t slop = 0.001f;
    auto penetration = std::min(overlap_x, overlap_y);
    if (overlap_x < overlap_y) {
        normal.x = m_Position.x > other->m_Position.x ? 1 : -1;
    } else {
        normal.y = m_Position.y > other->m_Position.y ? 1 : -1;
    }
    auto correction = std::max(penetration - slop, 0.0f) * percent * normal;
    auto rv = other->m_Velocity - m_Velocity;
    auto velAlongNormal = glm::dot(rv, normal);
    auto j = -velAlongNormal;
    auto impulse = j * normal;
    m_Velocity -= impulse;
    other->m_Velocity += impulse;
    m_Position += correction;
    other->m_Position -= correction;
}
void Enemy::CollisionWith(const std::shared_ptr<Character> &other) {
    if (IsDead())
        return;
    other->Hurt(m_["power"]);
    auto overlap_x =
        std::min(m_Position.x + Width() - other->GetPosition().x,
                 other->GetPosition().x + other->Width() - m_Position.x);
    auto overlap_y =
        std::min(m_Position.y + Height() - other->GetPosition().y,
                 other->GetPosition().y + other->Height() - m_Position.y);
    glm::vec2 normal{0, 0};
    if (overlap_x < overlap_y) {
        normal.x = m_Position.x > other->GetPosition().x ? 1 : -1;
    } else {
        normal.y = m_Position.y > other->GetPosition().y ? 1 : -1;
    }
    auto rv = other->GetVelocity() - m_Velocity;
    auto velAlongNormal = glm::dot(rv, normal);
    auto j = -velAlongNormal;
    auto impulse = j * normal * 1.5f;
    m_Velocity -= impulse;
    static float_t percent = 0.8f;
    static float_t slop = 0.001f;
    auto penetration = std::min(overlap_x, overlap_y);
    auto correction = std::max(penetration - slop, 0.0f) * percent * normal;
    m_Position += correction;
}
void Enemy::CollisionWith(
    const std::shared_ptr<Projectile::Projectile> &other) {
    if (IsDead() || !HitBy(other, other->Get("power"))) {
        return;
    }
    other->Set("penetrating", other->Get("penetrating") - 1);
    auto overlap_x =
        std::min(m_Position.x + Width() - other->GetPosition().x,
                 other->GetPosition().x + other->Width() - m_Position.x);
    auto overlap_y =
        std::min(m_Position.y + Height() - other->GetPosition().y,
                 other->GetPosition().y + other->Height() - m_Position.y);
    glm::vec2 normal{0, 0};
    if (overlap_x < overlap_y) {
        normal.x = m_Position.x > other->GetPosition().x ? 1 : -1;
    } else {
        normal.y = m_Position.y > other->GetPosition().y ? 1 : -1;
    }
    auto rv = other->GetVelocity() - m_Velocity;
    auto velAlongNormal = glm::dot(rv, normal);
    auto j = -velAlongNormal;
    auto impulse = j * normal * (2.2f + other->Get("knockBack"));
    m_Velocity -= impulse;
    static float_t percent = 0.8f;
    static float_t slop = 0.001f;
    auto penetration = std::min(overlap_x, overlap_y);
    auto correction = std::max(penetration - slop, 0.0f) * percent * normal;
    m_Position += correction;
}
void Enemy::SetScale(int32_t level) {
    m_["maxHp"] += level * m_["scale"];
}
float_t Enemy::Height() const {
    if (m_CurrentAnimation != NULL_STRING)
        return Util::Animated::GetAnimation(m_CurrentAnimation)->GetWidth() *
               std::abs(m_Transform.scale.x);
    return m_Drawable->GetSize().x * std::abs(m_Transform.scale.x);
}
float_t Enemy::Width() const {
    if (m_CurrentAnimation != NULL_STRING)
        return Util::Animated::GetAnimation(m_CurrentAnimation)->GetHeight() *
               m_Transform.scale.y;
    return m_Drawable->GetSize().y * m_Transform.scale.y;
}
float_t Enemy::Rotation() const {
    return m_Transform.rotation;
}
void Enemy::SetUp(std::string ID,
                  std::unordered_map<std::string, float_t> stat) {
    m_ID = ID;
    m_ = stat;
}

} // namespace Game::Enemy
