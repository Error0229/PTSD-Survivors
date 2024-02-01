#ifndef MANAGER_HPP
#define MANAGER_HPP
#include "Game/Character.hpp"
#include "Game/Enemy/Enemy.hpp"
#include "Game/Map.hpp"
#include "Game/Passive/Passive.hpp"
#include "Game/Projectile/Projectile.hpp"
#include "Game/Weapon/Weapon.hpp"

namespace Game {
class Manager {
public:
    void Start();
    void Update();
    void Draw();

private:
    std::vector<std::shared_ptr<Weapon::Weapon>> m_Weapons;
    std::vector<std::shared_ptr<Passive::Passive>> m_Passives;
    std::set<std::shared_ptr<Projectile::Projectile>> m_Projectiles;
    std::set<std::shared_ptr<Enemy::Enemy>> m_Enemies;
    std::shared_ptr<Character> m_Character;
    std::shared_ptr<Map> m_Map;
};
} // namespace Game

#endif
