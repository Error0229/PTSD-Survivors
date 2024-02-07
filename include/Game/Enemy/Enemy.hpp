#ifndef ENEMY_HPP
#define ENEMY_HPP
#include "Game/Util/Animated.hpp"
#include "Game/Util/ObjectPool.hpp"
#include "Game/Util/Physical.hpp"
#include "Util/GameObject.hpp"
#include "pch.hpp"
#include <cmath>
#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>

namespace Game::Enemy {
class Enemy : public ::Util::GameObject,
              public Util::Physical,
              public Util::Animated {
public:
    Enemy() = default;
    ~Enemy() override = default;
    void Start() override;
    void Update(const ::Util::Transform &transform = {}) override;
    void Draw() override;
    void Hurt(float_t damage);
    void SetScale(int32_t level);
    void SetUp(std::string ID, std::unordered_map<std::string, float_t> stat);
    void GoTo(glm::vec2);
    bool IsOver();
    bool IsDead();
    float_t Height() override;
    float_t Width() override;
    std::string ID();

private:
    std::string m_ID, m_Desc;
    std::unordered_map<std::string, float_t> m_;
    bool m_IsBoss, m_IsSwarm;
};
} // namespace Game::Enemy

#endif
