#include "Game/Enemy/Enemy.hpp"
#include "Game/Camera.hpp"
#include "Game/Util/Animated.hpp"
#include "Game/Util/Timer.hpp"
#include "Util/Time.hpp"
#include "pch.hpp"
#include <cmath>

namespace Game::Enemy {
void Enemy::Start() {
    m_["hp"] = m_["maxHp"];
    m_["isOver"] = false;
    m_["isDead"] = false;
    m_ZIndex = ENEMY_LAYER;
    Animated::Play();
}
void Enemy::Update(const ::Util::Transform &transform) {
    if (!IsDead())
        GoTo(transform.translation);
    Util::Animated::Update();
}
void Enemy::GoTo(glm::vec2 target) {
    m_Position += m_Velocity * static_cast<float>(::Util::Time::GetDeltaTime());
    auto direction = glm::normalize(target - m_Position);
    m_Velocity = direction * m_["speed"];
    m_Transform.translation = Camera::WorldToScreen(m_Position);
}
void Enemy::Draw() {
    Animated::Draw(m_Transform, m_ZIndex);
    if (IsDead() && !IsAnimated()) {
        m_["isOver"] = true;
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
void Enemy ::CollisionWith(const std::shared_ptr<Enemy> &other) {
    auto overlap_x =
        std::min(m_Position.x + Width() - other->m_Position.x,
                 other->m_Position.x + other->Width() - m_Position.x);
    auto overlap_y =
        std::min(m_Position.y + Height() - other->m_Position.y,
                 other->m_Position.y + other->Height() - m_Position.y);
    glm::vec2 normal{0, 0};
    static float_t percent = 0.5f;
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
    auto impulse = j * normal * 2.0f;
    m_Velocity -= impulse;
    static float_t percent = 0.9f;
    static float_t slop = 0.001f;
    auto penetration = std::min(overlap_x, overlap_y);
    auto correction = std::max(penetration - slop, 0.0f) * percent * normal;
    m_Position += correction;
}
void Enemy::SetScale(int32_t level) {
    m_["maxHp"] += level * m_["scale"];
}
float_t Enemy::Height() {
    if (m_CurrentAnimation != NULL_STRING)
        return Util::Animated::GetAnimation(m_CurrentAnimation)->GetWidth() *
               m_Transform.scale.x;
    return m_Drawable->GetSize().x * m_Transform.scale.x;
}
float_t Enemy::Width() {
    if (m_CurrentAnimation != NULL_STRING)
        return Util::Animated::GetAnimation(m_CurrentAnimation)->GetHeight() *
               m_Transform.scale.y;
    return m_Drawable->GetSize().y * m_Transform.scale.y;
}
void Enemy::SetUp(std::string ID,
                  std::unordered_map<std::string, float_t> stat) {
    m_ID = ID;
    m_ = stat;
}

} // namespace Game::Enemy
