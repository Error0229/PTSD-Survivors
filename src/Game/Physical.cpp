#include "Game/Physical.hpp"

namespace Game {

Physical::Physical() {
    this->m_Position = {0, 0};
    this->m_Size = {0, 0};
}

glm::vec2 &Physical::GetPosition() {
    return this->m_Position;
}

const std::type_info &Physical::Type() {
    return typeid(*this);
}

void Physical::SetVelocity(float velocity) {
    this->m_Velocity = velocity;
}

float Physical::Distance(std::shared_ptr<Physical> &other) {
    return glm::distance(this->m_Position, other->GetPosition());
}

bool Physical::IsCollideWith(std::shared_ptr<Physical> &other) {
    return Game::IsOverlapped(this->m_Position.x, this->m_Position.y,
                              this->m_Size.w, this->m_Size.h,
                              other->GetPosition().x, other->GetPosition().y,
                              other->Width(), other->Height());
}

void Physical::SetPosition(glm::vec2 position) {
    this->m_Position = position;
}

} // namespace Game
