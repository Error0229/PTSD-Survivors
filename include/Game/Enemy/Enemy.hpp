#ifndef ENEMY_HPP
#define ENEMY_HPP
#include "Game/Character.hpp"
#include "Game/Projectile/Projectile.hpp"
#include "Game/Util/Animated.hpp"
#include "Game/Util/Mirrored.hpp"
#include "Game/Util/Physical.hpp"
#include "Util/GameObject.hpp"
#include "pch.hpp"
#include <memory>

namespace Game::Enemy {
class Enemy : public ::Util::GameObject,
              public Util::Physical,
              public Util::Animated,
              public Util::Mirrored {
public:
    Enemy() = default;
    ~Enemy() override = default;
    void Start();
    void Update(const ::Util::Transform &transform = {});
    void Draw() override;
    void Hurt(float_t damage);
    bool HitBy(std::shared_ptr<Projectile::Projectile> proj,
               float_t damage); // if hit return true
    void SetScale(int32_t level);
    void SetUp(std::string ID, std::unordered_map<std::string, float_t> stat);
    void GoTo(glm::vec2 destination);
    void CollisionWith(const std::shared_ptr<Enemy> &other);
    void CollisionWith(const std::shared_ptr<Character> &other);
    void CollisionWith(const std::shared_ptr<Projectile::Projectile> &other);
    bool IsOver();
    bool IsDead();
    bool IsBoss();
    bool IsSwarm();
    float_t Height() const override;
    float_t Width() const override;
    float_t Rotation() const override;
    std::string ID();

private:
    time_t m_lastHit;
    bool m_IsStunned;
    std::string m_ID, m_Desc;
    std::unordered_map<std::string, float_t> m_;
    std::unordered_map<std::shared_ptr<Projectile::Projectile>, time_t>
        m_lastHitBy;
};
} // namespace Game::Enemy

#endif
