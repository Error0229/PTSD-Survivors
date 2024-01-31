#include "Game/Weapon/Weapon.hpp"

namespace Game::Weapon {
int32_t Weapon::GetLevel() {
    return m_Level;
}
int32_t Weapon::GetMaxLevel() {
    return m_MaxLevel;
}
std::string Weapon::GetLevelUpMessage() {
    return m_LevelUpMessage[m_Level];
}

bool Weapon::IsMaxLevel() {
    return m_Level == m_MaxLevel;
}
bool Weapon::IsEvo() {
    return m_EvoType != Type::NONE;
}
bool Weapon::CanEvo() {
    return m_Level == m_MaxLevel && m_EvoType != Type::NONE;
}

void Weapon::RecalculateStat() {
    m_Damage = m_BaseState.Damage * m_Modifier.Damage;
    m_Area = m_BaseState.Area * m_Modifier.Area;
    m_Speed = m_BaseState.Speed * m_Modifier.Speed;
    m_Amount = m_BaseState.Amount + m_Modifier.Amount;
    m_Duration = m_BaseState.Duration * m_Modifier.Duration;
    m_CoolDown = m_BaseState.CoolDown * m_Modifier.CoolDown;
}

void Weapon::UpdateModifier(DynamicStat &modifier) {
    m_Modifier = modifier;
    RecalculateStat();
}

void Weapon::Initialize() {
    // TODO: load base weapon
}

std::map<Type, std::shared_ptr<Weapon>> Weapon::s_BaseWeapons =
    std::map<Type, std::shared_ptr<Weapon>>();
std::vector<std::shared_ptr<Weapon>> Weapon::s_Weapons =
    std::vector<std::shared_ptr<Weapon>>();
} // namespace Game::Weapon
