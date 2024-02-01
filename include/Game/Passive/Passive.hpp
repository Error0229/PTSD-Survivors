#ifndef PASSIVE_HPP
#define PASSIVE_HPP
#include "Util/GameObject.hpp"
#include "pch.hpp"
#include <cmath>
#include <cstdint>
namespace Game::Passive {
enum class PassiveType;
enum class StackingType;
class Passive : public ::Util::GameObject {
public:
    Passive() = default;
    virtual ~Passive() = 0;
    virtual void Start() override;
    virtual void
    Update(const ::Util::Transform &transform = ::Util::Transform()) override;
    virtual void Draw() override;
    void LevelUp();
    int32_t GetLevel();
    int32_t GetMaxLevel();
    float_t GetEffect(int32_t level);

protected:
    PassiveType m_Type;
    int32_t m_Level, m_Rarity, m_MaxLevel;
    std::vector<float_t> m_EffectByLevel;
    StackingType m_StackingType;
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
enum class StackingType { Additive = 1, Multiplicative };
} // namespace Game::Passive
#endif // PASSIVE_HPP
