#include "Game/Character.hpp"
#include "Game/Camera.hpp"
#include "Game/Config.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
namespace Game {
void Character::Start() {
    m_ZIndex = CHARACTER_LAYER;
    m_Velocity = 100; // hardcoded for now
}
void Character::Update(const Util::Transform &transform) {
    GoTo(Camera::ScreenToWorld(transform.translation));
    m_Drawable->Draw(m_Transform, m_ZIndex);
}
void Character::Setup(std::string Name) {
    m_Name = Name;
    m_Transform.translation = Camera::WorldToScreen(m_Position);
    m_Drawable = std::make_unique<Util::Image>(CHARACTER_SPRITE_PATH + Name +
                                               SPRITE_EXT);
}

void Character::GoTo(glm::vec2 target) {
    auto direction = glm::normalize(target - m_Position);
    m_Position +=
        direction * m_Velocity * static_cast<float>(Util::Time::GetDeltaTime());
    Camera::Update(m_Position);
    m_Transform.translation = Camera::WorldToScreen(
        m_Position); // actually translation will always be 0,0
}

float Character::Width() {
    return m_Drawable->GetSize().x * m_Transform.scale.x;
}

float Character::Height() {
    return m_Drawable->GetSize().y * m_Transform.scale.y;
}
} // namespace Game
