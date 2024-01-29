#include "Game/Camera.hpp"

namespace Game{

Camera Camera();

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

}
