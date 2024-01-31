#include "Game/Util/Physical.hpp"

namespace Game::Util {

Physical::Physical() {
    m_Position = {0, 0};
}

glm::vec2 &Physical::GetPosition() {
    return m_Position;
}

const std::type_info &Physical::Type() {
    return typeid(*this);
}

void Physical::SetVelocity(float velocity) {
    m_Velocity = velocity;
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

} // namespace Game::Util
