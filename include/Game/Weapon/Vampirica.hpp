#ifndef VAMPIRICA_HPP
#define VAMPIRICA_HPP
#include "Game/Weapon/Weapon.hpp"
#include "Game/Weapon/Whip.hpp"
namespace Game::Weapon {
class Vampirica : public Whip {
public:
    explicit Vampirica(const Weapon &weapon) : Whip(weapon) {}
    void Update(const ::Util::Transform &transform) override;
};
} // namespace Game::Weapon

#endif
