#include "Game/Enemy/Enemy.hpp"
#include "Game/Util/Animated.hpp"
#include <cmath>

namespace Game::Enemy {
void Enemy::Start() {}
void Enemy::Update(const ::Util::Transform &transform) {}
void Enemy::Draw() {}
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
