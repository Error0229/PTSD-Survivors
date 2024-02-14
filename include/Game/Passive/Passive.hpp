#ifndef PASSIVE_HPP
#define PASSIVE_HPP
#include "Util/GameObject.hpp"
#include "pch.hpp"
#include <cmath>
#include <cstdint>
#include <unordered_set>
namespace Game::Passive {
enum class PassiveType;
class Passive : public ::Util::GameObject {
public:
    Passive() = default;
    ~Passive() override = default;
    virtual void Start();
    virtual void
    Update(const ::Util::Transform &transform = ::Util::Transform());
    virtual void Draw() override;
    void LevelUp();
    int32_t GetLevel();
    int32_t GetMaxLevel();
    void SetUp(
        const std::string &ID, const std::string &Description,
        std::unordered_map<std::string, float_t> &BaseStats,
        std::vector<std::vector<std::pair<std::string, float_t>>> &LevelUpStat);

    static float_t GetEffect(const std::string &name);
    static void Initialize();
    static bool IsEffect(const std::string &name);

protected:
    static std::unordered_map<std::string, float_t> s_Effect;
    static std::unordered_set<std::string> s_EffectName;
    std::string m_ID, m_Description;
    std::unordered_map<std::string, float_t> m_;
    std::vector<std::vector<std::pair<std::string, float_t>>> m_LevelUpStat;
};

enum class PassiveType {
    POWER,
    ARMOR,
    MAXHEALTH,
    REGEN,
    COOLDOWN,
    AREA,
    SPEED,
    DURATION,
    AMOUNT,
    MOVESPEED,
    MAGNET,
    LUCK,
    GROWTH,
    GREED,
    REVIVAL,
    CURSE,
    SILVER,
    GOLD,
    pLEFT,
    pRIGHT,
    PANDORA
};
} // namespace Game::Passive
#endif // PASSIVE_HPP
