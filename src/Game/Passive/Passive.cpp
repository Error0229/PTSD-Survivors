#include "Game/Passive/Passive.hpp"
#include <unordered_set>

namespace Game::Passive {
std::unordered_map<std::string, float_t> Passive::s_Effect;
std::unordered_set<std::string> Passive::s_EffectName;
void Passive::Start() {
    m_["level"] = 1;
    m_["maxLevel"] = m_LevelUpStat.size();
}
void Passive::Update(const ::Util::Transform &transform) {
    (void)transform;
}

void Passive::Draw() {
    // TODO: Implement Draw
}

void Passive::LevelUp() {
    for (auto &stat : m_LevelUpStat[m_["level"] - 1]) {
        m_[stat.first] += stat.second;
    }
    m_["level"] += 1;
}
int32_t Passive::GetLevel() {
    return m_["level"];
}
int32_t Passive::GetMaxLevel() {
    return m_["maxLevel"];
}
float_t Passive::GetEffect(const std::string &name) {
    return s_Effect[name];
}
void Passive::SetUp(
    const std::string &ID, const std::string &Description,
    std::unordered_map<std::string, float_t> &BaseStats,
    std::vector<std::vector<std::pair<std::string, float_t>>> &LevelUpStat) {
    m_ID = ID;
    m_Description = Description;
    m_ = BaseStats;
    m_LevelUpStat = LevelUpStat;
}
void Passive::Initialize() {
    s_Effect["power"] = 0.0f;
    s_Effect["armor"] = 0.0f;
    s_Effect["maxHp"] = 0.0f;
    s_Effect["regen"] = 0.0f;
    s_Effect["coolDown"] = 0.0f;
    s_Effect["area"] = 0.0f;
    s_Effect["speed"] = 0.0f;
    s_Effect["duration"] = 0.0f;
    s_Effect["interval"] = 0.0f; // cooldown for weapon
    s_Effect["amount"] = 0.0f;
    s_Effect["moveSpeed"] = 0.0f;
    s_Effect["magnet"] = 0.0f;
    s_Effect["luck"] = 0.0f;
    s_Effect["growth"] = 0.0f;
    s_Effect["greed"] = 0.0f;
    s_Effect["curse"] = 0.0f;
    s_Effect["revivals"] = 0.0f;
    s_Effect["penetrating"] = 0.0f; // pierce
    for (auto &effect : s_Effect) {
        s_EffectName.insert(effect.first);
    }
}
bool Passive::IsEffect(const std::string &name) {
    return s_EffectName.count(name) > 0;
}
}; // namespace Game::Passive
