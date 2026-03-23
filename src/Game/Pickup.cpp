#include "Game/Pickup.hpp"
#include "Game/Camera.hpp"
#include "Game/Config.hpp"

namespace Game {

void Pickup::Spawn(Type type, const glm::vec2 &position) {
    m_Type = type;
    m_Collected = false;
    m_Attracted = false;
    m_Lifetime = 60.0f;
    m_AttractionSpeed = 0;
    m_Position = position;

    switch (type) {
    case Type::XP_SMALL:
        m_Value = 1.0f;
        break;
    case Type::XP_MED:
        m_Value = 3.0f;
        break;
    case Type::XP_LARGE:
        m_Value = 10.0f;
        break;
    case Type::COIN:
        m_Value = 1.0f;
        break;
    case Type::CHEST:
        m_Value = 0.0f;
        break;
    case Type::CHICKEN:
        m_Value = 30.0f;
        break;
    case Type::ROSARY:
        m_Value = 0.0f;
        break;
    case Type::VACUUM:
        m_Value = 0.0f;
        break;
    }
}

void Pickup::Update(float dt, const glm::vec2 &playerPos, float magnetRange) {
    if (m_Collected)
        return;

    m_Lifetime -= dt;

    float dist = glm::distance(m_Position, playerPos);

    // Magnet attraction (flat distance — not in QuadTree per eng review)
    if (!m_Attracted && dist < magnetRange) {
        m_Attracted = true;
        m_AttractionSpeed = 200.0f;
    }

    // Accelerate toward player when attracted
    if (m_Attracted) {
        m_AttractionSpeed += 800.0f * dt;
        glm::vec2 dir = playerPos - m_Position;
        float len = glm::length(dir);
        if (len > 0.001f) {
            dir /= len;
            m_Position += dir * m_AttractionSpeed * dt;
        }
    }

    // Collection on contact
    if (dist < COLLECTION_RADIUS) {
        m_Collected = true;
    }
}

void Pickup::Draw() {
    if (m_Collected)
        return;
    // Pickup rendering will use sprites in Phase 5/6.
    // For now, pickups exist as invisible collectibles (logic-only).
    // The XP/collection mechanics work without rendering.
}

} // namespace Game
