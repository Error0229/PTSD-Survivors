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

void Passive::Initialize() {
    // TODO: load base passive
}

int32_t Passive::PassiveCount() {
    return s_Passives.size();
}

void Passive::LevelUp(PassiveType type) {
    for (auto &passive : s_Passives) {
        if (passive->m_Type == type) {
            passive->LevelUp();
            break;
        }
    }
}
std::vector<std::shared_ptr<Passive>> Passive::s_Passives =
    std::vector<std::shared_ptr<Passive>>();
std::vector<std::shared_ptr<Passive>> Passive::s_BasePassives =
    std::vector<std::shared_ptr<Passive>>();
}; // namespace Game::Passive
