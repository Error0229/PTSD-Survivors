#ifndef WHIP_HPP
#define WHIP_HPP

#include "Game/Projectile/Projectile.hpp"
#include "Game/Weapon/Weapon.hpp"
#include "pch.hpp"
namespace Game::Weapon {
class Whip : public Weapon {
public:
    Whip() = default;
    explicit Whip(const Weapon &weapon) : Weapon(weapon) {}
    void Update(const ::Util::Transform &transform) override;
};
} // namespace Game::Weapon
#endif /* WHIP_HPP */
