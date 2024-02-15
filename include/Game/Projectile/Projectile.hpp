#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP
#include "Game/Util/Animated.hpp"
#include "Game/Util/Mirrored.hpp"
#include "Game/Util/Physical.hpp"
#include "Game/Weapon/Weapon.hpp"
#include "Util/GameObject.hpp"
#include "pch.hpp"
#include <cmath>
namespace Game::Projectile {
struct Stats {
    int32_t duration, pierce, poolLimit, blockByWall;
    float_t area, chance, critMulti, damage, knockBack, angle;
    time_t createdTime, interval, hitboxDelay, delay;
};
class Projectile : public ::Util::GameObject,
                   public Util::Physical,
                   public Util::Animated,
                   public Util::Mirrored {
public:
    Projectile() = default;
    Projectile(std::string ID);
    virtual ~Projectile() = default;
    bool operator<(const Projectile &rhs) const;
    virtual void Start();
    virtual void
    Update(const ::Util::Transform &transform = ::Util::Transform());
    virtual void Draw() override;
    bool IsOver() const;
    void SetUp(std::unordered_map<std::string, float_t> stats);
    float_t Height() const override;
    float_t Width() const override;
    float_t Rotation() const override;
    void SetDelay(time_t delay);
    void CollideWith(std::shared_ptr<::Util::GameObject> &other);

protected:
    float m_ZIndex = PROJECTILE_LAYER;
    std::unordered_map<std::string, float_t> m_;
    std::string m_ID;
    bool m_IsOver = false, m_IsStarted = false;
    time_t m_StartTime, m_Delay, m_CreateTime;
};

} // namespace Game::Projectile
#endif // PROJECTILE_HPP
