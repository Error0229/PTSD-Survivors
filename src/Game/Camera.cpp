#include "Game/Camera.hpp"

namespace Game {

Camera Camera();
glm::vec2 Camera::Position = {0, 0};
void Camera::Update(const glm::vec2 &position) {
    Position = position;
}

glm::vec2 Camera::GetPosition() {
    return Position;
}

glm::vec2 Camera::ScreenToWorld(glm::vec2 screenPosition) {
    return Position + screenPosition;
}

glm::vec2 Camera::WorldToScreen(glm::vec2 worldPosition) {
    return worldPosition - Position;
}

} // namespace Game
