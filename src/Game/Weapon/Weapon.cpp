#include "Game/Weapon/Weapon.hpp"
#include "Game/Manager.hpp"
#include <unordered_map>

namespace Game::Weapon {
void Weapon::SetUp(
    std::string ID, std::string Description,
    std::vector<std::string> EvoRequired, std::vector<std::string> EvoFrom,
    std::unordered_map<std::string, float_t> BaseStats,
    std::vector<std::vector<std::pair<std::string, float_t>>> LevelUpStat) {
    m_ID = ID;
    m_Description = Description;
    m_Base = BaseStats;
    m_ = BaseStats;
    m_LevelUpStat = LevelUpStat;
    m_EvoRequired = EvoRequired;
    m_EvoFrom = EvoFrom;
    m_["level"] = 0;
    m_["maxLevel"] = m_LevelUpStat.size();
}

int32_t Weapon::GetLevel() {
    return static_cast<int32_t>(m_["level"]);
}
int32_t Weapon::GetMaxLevel() {
    return static_cast<int32_t>(m_["maxLevel"]);
}
std::string Weapon::GetLevelUpMessage() {
    return m_LevelUpMessage[GetLevel() - 1];
}

void Weapon::Start() {
    m_["level"] = 1;
    m_["maxLevel"] = m_LevelUpStat.size();
    RecalculateStat();
}

void Weapon::Update(const ::Util::Transform &transform) {
    (void)transform;
}

void Weapon::Draw() {}

void Weapon::LevelUp() {
    assert(m_["level"] < m_["maxLevel"]);
    for (auto &stat : m_LevelUpStat[m_["level"] - 1]) {
        m_[stat.first] += stat.second;
    }
    m_["level"] += 1;
    RecalculateStat();
}

bool Weapon::IsMaxLevel() {
    return m_["level"] == m_["maxLevel"];
}
bool Weapon::IsEvo() {
    return m_["isEvolution"] == 1.0f;
}
bool Weapon::CanEvo() { // for evo-able weapon to call
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
    m_["damage"] = m_Base["damage"] * m_Mod["damage"];
    m_["area"] = m_Base["area"] * m_Mod["area"];
    m_["speed"] = m_Base["speed"] * m_Mod["speed"];
    m_["amount"] = m_Base["amount"] + m_Mod["amount"];
    m_["duration"] = m_Base["duration"] * m_Mod["duration"];
    m_["coolDown"] = m_Base["coolDown"] * m_Mod["coolDown"];
}

void Weapon::UpdateModifier(
    std::unordered_map<std::string, float_t> &modifier) {
    m_Mod = modifier;
    RecalculateStat();
}
} // namespace Game::Weapon
