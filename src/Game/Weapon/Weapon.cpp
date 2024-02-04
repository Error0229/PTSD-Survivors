#include "Game/Weapon/Weapon.hpp"
#include "Game/Manager.hpp"
#include <unordered_map>

namespace Game::Weapon {
int32_t Weapon::GetLevel() {
    return static_cast<int32_t>(m_["Level"]);
}
int32_t Weapon::GetMaxLevel() {
    return static_cast<int32_t>(m_["MaxLevel"]);
}
std::string Weapon::GetLevelUpMessage() {
    return m_LevelUpMessage[GetLevel() - 1];
}

bool Weapon::IsMaxLevel() {
    return m_["Level"] == m_["MaxLevel"];
}
bool Weapon::IsEvo() {
    return m_["IsEvolution"] == 1.0f;
}
bool Weapon::CanEvo() {
    if (!IsMaxLevel())
        return false;
    for (auto &evo : m_EvoRequired) {
        if (!Game::π.Have(evo))
            return false;
    }
    for (auto &evo : m_EvoFrom) {
        if (!Game::π.Have(evo))
            return false;
    }
    return true;
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
