#include "Game/Weapon/Weapon.hpp"
#include <unordered_map>

namespace Game::Weapon {
int32_t Weapon::GetLevel() {
    return static_cast<int32_t>(m_["level"]);
}
int32_t Weapon::GetMaxLevel() {
    return m_["maxLevel"];
}
std::string Weapon::GetLevelUpMessage() {
    return m_LevelUpMessage[GetLevel() - 1];
}

bool Weapon::IsMaxLevel() {
    return m_["level"] == m_["maxLevel"];
}
bool Weapon::IsEvo() {
    return m_["isEvolution"] == 1.0f;
}
bool Weapon::CanEvo() {
    return m_["isEvolution"] && m_["level"] == m_["maxLevel"];
}

void Weapon::RecalculateStat() {
    m_["Damage"] = m_Base["Damage"] * m_Mod["Damage"];
    m_["Area"] = m_Base["Area"] * m_Mod["Area"];
    m_["Speed"] = m_Base["Speed"] * m_Mod["Speed"];
    m_["Amount"] = m_Base["Amount"] + m_Mod["Amount"];
    m_["Duration"] = m_Base["Duration"] * m_Mod["Duration"];
    m_["CoolDown"] = m_Base["CoolDown"] * m_Mod["CoolDown"];
}

void Weapon::UpdateModifier(
    std::unordered_map<std::string, float_t> &modifier) {
    m_Mod = modifier;
    RecalculateStat();
}
} // namespace Game::Weapon
