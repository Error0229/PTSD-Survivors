#include "Game/Camera.hpp"

namespace Game {

Camera Camera();
glm::vec2 Camera::s_Position = {0, 0};
void Camera::Update(const glm::vec2 &position) {
    s_Position = position;
}

glm::vec2 Camera::GetPosition() {
    return s_Position;
}

glm::vec2 Camera::ScreenToWorld(glm::vec2 screenPosition) {
    return s_Position + screenPosition;
}

glm::vec2 Camera::WorldToScreen(glm::vec2 worldPosition) {
    return worldPosition - s_Position;
}

} // namespace Game
