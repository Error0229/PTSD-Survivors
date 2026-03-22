#ifndef WEAPON_HPP
#define WEAPON_HPP
#include "Game/Passive/Passive.hpp"
#include "Game/Projectile/Behavior.hpp"
#include "Game/Projectile/Projectile.hpp"
#include "Util/GameObject.hpp"
#include "pch.hpp"
#include <cmath>
#include <functional>
#include <memory>
#include <string>
namespace Game {
namespace Weapon {
enum class Type;

// Maps weapon ID string to the behavior variant it should use
Projectile::BehaviorVariant CreateBehaviorForWeapon(const std::string &weaponID);

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

    void Start();
    void Update(const ::Util::Transform &transform = ::Util::Transform());
    void Draw() override;
    void LevelUp();
    std::string ID();

    // Evolution data accessors
    const std::vector<std::string> &GetEvoRequired() const {
        return m_EvoRequired;
    }
    const std::vector<std::string> &GetEvoFrom() const { return m_EvoFrom; }

protected:
    std::string m_ID, m_Description;
    time_t m_LastTimeAttack = -1;
    // m_ indicate the stat
    std::unordered_map<std::string, float_t> m_, m_Base, m_Mod;
    std::vector<std::string> m_LevelUpMessage;
    std::vector<std::vector<std::pair<std::string, float_t>>> m_LevelUpStat;
    std::vector<std::string> m_EvoRequired, m_EvoFrom;

private:
    void FireProjectiles();
    bool m_PermanentFired = false;
};

} // namespace Weapon
} // namespace Game
#endif // WEAPON_HPP
