#include "Game/Util/Mirrored.hpp"

namespace Game::Util {
bool Mirrored::IsMirrored() const {
    return m_Direction != m_DefaultDirection;
}
void Mirrored::SetDirection(Direction direction) {
    m_Direction = direction;
}
Direction Mirrored::GetDirection() const {
    return m_Direction;
}
} // namespace Game::Util
