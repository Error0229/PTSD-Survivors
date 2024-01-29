#include "Game/Character.hpp"
#include "Game/Camera.hpp"
#include "Game/Config.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
namespace Game {
void Character::Start() {
    m_Position = {0, 0};
    m_ZIndex = CHARACTER_LAYER;
}
void Character::SetSpeed(float speed) {
    m_Speed = speed;
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
        direction * m_Speed * static_cast<float>(Util::Time::GetDeltaTime());
    Camera::Update(m_Position);
    m_Transform.translation = Camera::WorldToScreen(
        m_Position); // actually translation will always be 0,0
}

glm::vec2 Character::GetPosition() {
    return m_Position;
}

} // namespace Game
