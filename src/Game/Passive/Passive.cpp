#include "Game/Passive/Passive.hpp"

namespace Game::Passive {
void Passive::LevelUp() {
    m_Level++;
}
int32_t Passive::GetLevel() {
    return m_Level;
}
int32_t Passive::GetMaxLevel() {
    return m_MaxLevel;
}
float_t Passive::GetEffect(int32_t level) {
    return m_EffectByLevel[level] - m_EffectByLevel[level - 1];
}

}; // namespace Game::Passive
