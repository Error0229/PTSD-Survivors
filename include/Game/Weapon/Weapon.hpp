#ifndef WEAPON_HPP
#define WEAPON_HPP
#include "Game/Passive/Passive.hpp"
#include "Util/GameObject.hpp"
#include "pch.hpp"
#include <cmath>
#include <string>
namespace Game {
namespace Weapon {
enum class Type;
class Weapon : public ::Util::GameObject {
public:
    Weapon() = default;

    int32_t GetLevel();
    int32_t GetMaxLevel();
    std::string GetLevelUpMessage();

    void SetUp(
        std::string ID, std::string Description,
        std::vector<std::string> EvoRequired, std::vector<std::string> EvoFrom,
        std::unordered_map<std::string, float_t> BaseStats,
        std::vector<std::vector<std::pair<std::string, float_t>>> LevelUpStat);

    bool IsMaxLevel();
    bool IsEvo();
    bool CanEvo();
    void RecalculateStat();
    void UpdateModifier(std::unordered_map<std::string, float_t> &modifier);

    virtual void Start();
    virtual void
    Update(const ::Util::Transform &transform = ::Util::Transform());
    virtual void Draw() override;
    virtual void LevelUp();
    std::string ID();

protected:
    std::string m_ID, m_Description;
    time_t m_LastTimeAttack;
    // m_ indicate the stat
    std::unordered_map<std::string, float_t> m_, m_Base, m_Mod;
    std::vector<std::string> m_LevelUpMessage;
    std::vector<std::vector<std::pair<std::string, float_t>>> m_LevelUpStat;
    std::vector<std::string> m_EvoRequired, m_EvoFrom;
};

} // namespace Weapon
} // namespace Game
#endif // WEAPON_HPP
