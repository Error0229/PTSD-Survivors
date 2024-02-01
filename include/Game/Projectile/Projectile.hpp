#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP
#include "Game/Util/Physical.hpp"
#include "Game/Weapon/Weapon.hpp"
#include "Util/GameObject.hpp"
#include "pch.hpp"
#include <cmath>
namespace Game::Projectile {
struct Stats {
    int32_t duration, pierce, poolLimit, blockByWall;
    float_t area, chance, critMulti, damage, knockBack, angle;
    clock_t createdTime, interval, hitboxDelay, delay;
};
class Projectile : public ::Util::GameObject, public Util::Physical {
public:
    Projectile() = default;
    virtual ~Projectile() = 0;
    bool operator<(const Projectile &rhs) const;
    virtual void Start() override;
    virtual void
    Update(const ::Util::Transform &transform = ::Util::Transform()) override;
    virtual void Draw() override;

    void SetStatus(Stats stats);
    float_t Height() override;
    float_t Width() override;

    void CollideWith(std::shared_ptr<::Util::GameObject> &other);

    static void Initialize();
    static void Reset();

    static std::set<std::shared_ptr<Projectile>> s_Projectiles;
    // static object pool for derived class
protected:
    Stats m_Stats;
    Game::Weapon::Type m_Type;
    bool m_IsOver = false, m_IsStarted = false;
};

} // namespace Game::Projectile

#endif // PROJECTILE_HPP
