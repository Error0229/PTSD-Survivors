#include "Game/Character.hpp"
#include "Game/Camera.hpp"
#include "Game/Config.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
namespace Game {
void Character::Start() {
    m_Position = {0, 0};
}
void Character::SetSpeed(float speed) {
    m_Speed = speed;
}
void Character::Update(const Util::Transform &transform) {
    auto direction =
        glm::normalize(transform.translation - m_Transform.translation);
    m_Position +=
        direction * m_Speed * static_cast<float>(Util::Time::GetDeltaTime());
    m_Transform.translation = Camera::WorldToScreen(m_Position);
    m_Drawable->Draw(m_Transform, m_ZIndex);
}
void Character::Setup(std::string Name) {
    m_Name = Name;
    m_Drawable = std::make_unique<Util::Image>(CHARACTER_SPRITE_PATH + Name +
                                               SPRITE_EXT);
}
} // namespace Game
