#ifndef PICKUP_HPP
#define PICKUP_HPP

#include "pch.hpp"

namespace Game {

// Lightweight pickup — NOT a GameObject. Uses flat distance check, not QuadTree.
class Pickup {
public:
    enum class Type {
        XP_SMALL,  // +1 XP (blue gem)
        XP_MED,    // +3 XP (green gem)
        XP_LARGE,  // +10 XP (red gem)
        COIN,      // +1 coin
        CHEST,     // boss drop — triggers evolution/rewards
        CHICKEN,   // heal 30 HP
        ROSARY,    // kill all on-screen enemies
        VACUUM,    // collect all on-screen pickups
    };

    void Spawn(Type type, const glm::vec2 &position);
    void Update(float dt, const glm::vec2 &playerPos, float magnetRange);
    void Draw();

    bool IsCollected() const { return m_Collected; }
    bool IsExpired() const { return m_Lifetime <= 0; }
    bool ShouldRemove() const { return m_Collected || m_Lifetime <= 0; }
    Type GetType() const { return m_Type; }
    float GetValue() const { return m_Value; }
    float GetEvoChance() const { return m_EvoChance; }
    void SetEvoChance(float chance) { m_EvoChance = chance; }
    glm::vec2 GetPosition() const { return m_Position; }

private:
    Type m_Type = Type::XP_SMALL;
    float m_Value = 1.0f;
    float m_EvoChance = 0.0f;
    bool m_Collected = false;
    bool m_Attracted = false;
    float m_Lifetime = 60.0f;
    float m_AttractionSpeed = 0;
    glm::vec2 m_Position{0, 0};
    static constexpr float COLLECTION_RADIUS = 20.0f;
};

} // namespace Game

#endif // PICKUP_HPP
