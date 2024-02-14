#include "Game/Util/Physical.hpp"

namespace Game::Util {

Physical::Physical() {
    m_Position = {0, 0};
}

glm::vec2 &Physical::GetPosition() {
    return m_Position;
}

glm::vec2 &Physical::GetVelocity() {
    return m_Velocity;
}

const std::type_info &Physical::Type() {
    return typeid(*this);
}

float Physical::Distance(std::shared_ptr<Physical> &other) {
    return glm::distance(m_Position, other->GetPosition());
}

bool Physical::IsCollideWith(std::shared_ptr<Physical> &other) {
    return IsOverlapped(m_Position.x, m_Position.y, Width(), Height(),
                        other->GetPosition().x, other->GetPosition().y,
                        other->Width(), other->Height());
}

void Physical::SetPosition(glm::vec2 position) {
    m_Position = position;
}
void Physical::SetVelocity(glm::vec2 velocity) {
    m_Velocity = velocity;
}

} // namespace Game::Util
