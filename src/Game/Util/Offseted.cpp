#include "Game/Util/Offsetted.hpp"

namespace Game::Util {
void Offsetted::SetOffset(const glm::vec2 &offset) {
    m_Offset = offset;
}
const glm::vec2 &Offsetted::GetOffset() const {
    return m_Offset;
}
} // namespace Game::Util
