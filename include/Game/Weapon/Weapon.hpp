#ifndef WEAPON_HPP
#define WEAPON_HPP
#include "Game/Passive/Passive.hpp"
#include "Util/GameObject.hpp"
#include "pch.hpp"
#include <cmath>
namespace Game::Weapon {
enum class Type;
class Weapon : public ::Util::GameObject {
public:
    Weapon() = default;

    int32_t GetLevel();
    int32_t GetMaxLevel();
    std::string GetLevelUpMessage();

    void SetUp(std::string ID, std::string EvoID, std::string Description,
               std::vector<std::string> EvoRequired,
               std::vector<std::string> EvoFrom,
               std::unordered_map<std::string, float_t> BaseStats,
               std::vector<std::pair<std::string, float_t>> LevelUpStat);

    bool IsMaxLevel();
    bool IsEvo();
    bool CanEvo();
    void RecalculateStat();
    void UpdateModifier(std::unordered_map<std::string, float_t> &modifier);

    virtual void Start() override;
    virtual void
    Update(const ::Util::Transform &transform = ::Util::Transform()) override;
    virtual void Draw() override;
    virtual void LevelUp() = 0;

protected:
    std::string m_ID, m_EvoID, m_Description;
    // m_ indicate the stat
    std::unordered_map<std::string, float_t> m_, m_Base, m_Mod;
    std::vector<std::string> m_LevelUpMessage;
    std::vector<std::pair<std::string, float_t>> m_LevelUpStat;
    std::vector<std::string> m_EvoRequired, m_EvoFrom;
};

} // namespace Game::Weapon
#endif // WEAPON_HPP
