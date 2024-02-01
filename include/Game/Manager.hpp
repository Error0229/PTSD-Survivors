#ifndef MANAGER_HPP
#define MANAGER_HPP
#include "Game/Character.hpp"
#include "Game/Map.hpp"
#include "Game/Passive/Passive.hpp"
#include "Game/Projectile/Projectile.hpp"
#include "Game/Weapon/Weapon.hpp"
#include "Util/GameObject.hpp"
namespace Game {
class Manager : public ::Util::GameObject {
public:
private:
    std::vector<std::shared_ptr<Weapon::Weapon>> m_Weapons;
    std::vector<std::shared_ptr<Passive::Passive>> m_Passives;
    std::set<std::shared_ptr<Projectile::Projectile>> m_Projectiles;
    std::shared_ptr<Character> m_Character;
    std::shared_ptr<Map> m_Map;
};
} // namespace Game

#endif
