#include "Game/Character.hpp"
#include "Game/Camera.hpp"
#include "Game/Config.hpp"
#include "Game/Util/Animated.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "pch.hpp"
namespace Game {
void Character::Start() {
    m_ZIndex = CHARACTER_LAYER;
    m_Transform.scale = {2, 2}; // hardcoded for Debug
    Util::Animated::Play();
    m_["hp"] = m_["maxHp"];
}
void Character::Update(const ::Util::Transform &transform) {
    GoTo(Camera::ScreenToWorld(transform.translation));
    Util::Animated::Update();
}

void Character::GoTo(glm::vec2 target) {
    auto direction = glm::normalize(target - m_Position);
    m_Velocity = direction * m_["moveSpeed"];
    m_Position +=
        m_Velocity * 100.0f * static_cast<float>(::Util::Time::GetDeltaTime());
    Camera::Update(m_Position);
    m_Transform.translation = Camera::WorldToScreen(
        m_Position); // actually translation will always be 0,0
}
void Character::Draw() {
    Util::Animated::Draw(m_Transform, m_ZIndex);
    // m_Drawable->Draw(m_Transform, m_ZIndex);
}
float Character::Width() {
    if (m_CurrentAnimation != NULL_STRING)
        return Util::Animated::GetAnimation(m_CurrentAnimation)->GetWidth() *
               m_Transform.scale.x;
    return m_Drawable->GetSize().x * m_Transform.scale.x;
}

float Character::Height() {
    if (m_CurrentAnimation != NULL_STRING)
        return Util::Animated::GetAnimation(m_CurrentAnimation)->GetHeight() *
               m_Transform.scale.y;
    return m_Drawable->GetSize().y * m_Transform.scale.y;
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

} // namespace Game
