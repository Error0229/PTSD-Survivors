#include "Game/Character.hpp"
#include "Game/Camera.hpp"
#include "Game/Config.hpp"
#include "Game/Util/Animated.hpp"
#include "Game/Util/Util.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "pch.hpp"
#include <cmath>
namespace Game {
void Character::Start() {
    m_Type = CHARACTER;
    m_DefaultDirection = Util::Direction::RIGHT;
    m_ZIndex = CHARACTER_LAYER;
    m_Transform.scale = {2, 2}; // hardcoded for Debug
    Util::Animated::Play();
    m_["hp"] = m_["maxHp"];
}
void Character::Update(const ::Util::Transform &transform) {
    GoTo(Camera::ScreenToWorld(transform.translation));
}

void Character::GoTo(glm::vec2 target) {
    if (target.x < m_Position.x) {
        m_Direction = Util::Direction::LEFT;
    } else {
        m_Direction = Util::Direction::RIGHT;
    }
    auto direction = glm::normalize(target - m_Position);
    m_Velocity = direction * m_["moveSpeed"];
    m_Position +=
        m_Velocity * 100.0f * static_cast<float>(::Util::Time::GetDeltaTime());
    Camera::Update(m_Position);
    m_Transform.translation = Camera::WorldToScreen(
        m_Position); // actually translation will always be 0,0
}
void Character::Draw() {
    if ((IsMirrored() && m_Transform.scale.x > 0) ||
        (!IsMirrored() && m_Transform.scale.x < 0)) {
        m_Transform.scale.x *= -1;
    }
    Util::Animated::Draw(m_Transform, m_ZIndex);
    // m_Drawable->Draw(m_Transform, m_ZIndex);
}
float_t Character::Width() const {
    if (m_CurrentAnimation != NULL_STRING)
        return Util::Animated::GetAnimation(m_CurrentAnimation)->GetWidth() *
               std::abs(m_Transform.scale.x);
    return m_Drawable->GetSize().x * std::abs(m_Transform.scale.x);
}

float_t Character::Height() const {
    if (m_CurrentAnimation != NULL_STRING)
        return Util::Animated::GetAnimation(m_CurrentAnimation)->GetHeight() *
               std::abs(m_Transform.scale.y);
    return m_Drawable->GetSize().y * std::abs(m_Transform.scale.y);
}
float_t Character::Rotation() const {
    return m_Transform.rotation;
}
void Character::SetBaseStats(std::unordered_map<std::string, float_t> &stats) {
    m_BaseStats = stats;
    RecalculateStats();
}

void Character::RecalculateStats() {
    m_ = m_BaseStats; // hardcoded for now
}
void Character::Hurt(float_t damage) {
    m_["hp"] -= damage;
    if (m_["hp"] <= 0) {
        /*m_["hp"] = 0;
        SetAnimation("Death");
        Util::Animated::Play();*/
    }
}

void Character::SetInfos(std::string ID, std::string chrName,
                         std::string description, std::string bgm,
                         std::string weapon) {
    m_ID = ID;
    m_ChrName = chrName;
    m_Description = description;
    m_BGM = bgm;
    m_Weapon = weapon;
}

void Character::SetSpeed(float speed) {
    m_["moveSpeed"] = speed;
}

// XP curve from OOPL-VS:
// Level 1-19:  base 5 + 10/level
// Level 20-39: base 205 + 13/level
// Level 40+:   base 475 + 16/level
float Character::ComputeMaxXP(int level) const {
    if (level < 20) {
        return 5.0f + 10.0f * level;
    } else if (level < 40) {
        return 205.0f + 13.0f * level;
    } else {
        return 475.0f + 16.0f * level;
    }
}

int Character::AddXP(float amount) {
    m_XP += amount;
    int levelsGained = 0;
    while (m_XP >= m_MaxXP) {
        m_XP -= m_MaxXP;
        m_Level++;
        m_MaxXP = ComputeMaxXP(m_Level);
        levelsGained++;
    }
    return levelsGained;
}

} // namespace Game
