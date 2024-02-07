#include "Game/Enemy/Enemy.hpp"
#include "Game/Camera.hpp"
#include "Game/Util/Animated.hpp"
#include "Game/Util/Timer.hpp"
#include "Util/Time.hpp"
#include "pch.hpp"
#include <cmath>

namespace Game::Enemy {
void Enemy::Start() {
    m_["Hp"] = m_["MaxHp"];
    m_["IsOver"] = false;
    m_["IsDead"] = false;
    m_ZIndex = ENEMY_LAYER;
    Animated::Play();
}
void Enemy::Update(const ::Util::Transform &transform) {
    if (!IsDead())
        GoTo(transform.translation);
    Util::Animated::Update();
}
void Enemy::GoTo(glm::vec2 target) {
    auto direction = glm::normalize(target - m_Position);
    m_Position += direction * m_["speed"] *
                  static_cast<float>(::Util::Time::GetDeltaTime());
    m_Transform.translation = Camera::WorldToScreen(m_Position);
}
void Enemy::Draw() {
    Animated::Draw(m_Transform, m_ZIndex);
    if (IsDead() && !IsAnimated()) {
        m_["IsOver"] = true;
    }
}
void Enemy::Hurt(float_t damage) {
    m_["Hp"] -= damage;
    if (m_["Hp"] <= 0 && !IsDead()) {
        m_["Hp"] = 0;
        SetAnimation("Death");
        m_["IsDead"] = true;
        Animated::Play();
    }
}
std::string Enemy::ID() {
    return m_ID;
}
bool Enemy::IsOver() {
    return m_["IsOver"];
}

bool Enemy::IsDead() {
    return m_["IsDead"];
}
void Enemy::SetScale(int32_t level) {
    m_["maxHp"] += level * m_["scale"];
}
float_t Enemy::Height() {
    if (m_CurrentAnimation != NULL_STRING)
        return Util::Animated::GetAnimation(m_CurrentAnimation)->GetWidth();
    return m_Drawable->GetSize().x * m_Transform.scale.x;
}
float_t Enemy::Width() {
    if (m_CurrentAnimation != NULL_STRING)
        return Util::Animated::GetAnimation(m_CurrentAnimation)->GetHeight();
    return m_Drawable->GetSize().y * m_Transform.scale.y;
}
void Enemy::SetUp(std::string ID,
                  std::unordered_map<std::string, float_t> stat) {
    m_ID = ID;
    m_ = stat;
}

} // namespace Game::Enemy
