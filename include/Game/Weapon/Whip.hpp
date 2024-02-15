#ifndef WHIP_HPP
#define WHIP_HPP

#include "Game/Weapon/Weapon.hpp"
namespace Game::Weapon {
class Whip : public Weapon {
public:
    explicit Whip(const Weapon &weapon) : Weapon(weapon) {}
    void Start() override;
    void Update(const ::Util::Transform &transform) override;
    void Draw() override;
};
} // namespace Game::Weapon
#endif /* WHIP_HPP */
