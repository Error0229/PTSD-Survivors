#include "Game/Character.hpp"
#include "Game/Camera.hpp"
#include "Game/Config.hpp"
#include "Game/Util/Animated.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
namespace Game {
void Character::Start() {
    m_ZIndex = CHARACTER_LAYER;
    Util::Animated::Play();
}
void Character::Update(const ::Util::Transform &transform) {
    GoTo(Camera::ScreenToWorld(transform.translation));
    Util::Animated::Update();
}

void Character::GoTo(glm::vec2 target) {
    auto direction = glm::normalize(target - m_Position);
    m_Position += direction * m_Stats["moveSpeed"] * 100.0f *
                  static_cast<float>(::Util::Time::GetDeltaTime());
    Camera::Update(m_Position);
    m_Transform.translation = Camera::WorldToScreen(
        m_Position); // actually translation will always be 0,0
}
void Character::Draw() {
    Util::Animated::Draw(m_Transform, m_ZIndex);
    // m_Drawable->Draw(m_Transform, m_ZIndex);
}
float Character::Width() {
    return m_Drawable->GetSize().x * m_Transform.scale.x;
}

float Character::Height() {
    return m_Drawable->GetSize().y * m_Transform.scale.y;
}

void Character::SetBaseStats(std::unordered_map<std::string, float_t> &stats) {
    m_BaseStats = stats;
    RecalculateStats();
}

void Character::RecalculateStats() {
	m_Stats = m_BaseStats; // hardcoded for now
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
