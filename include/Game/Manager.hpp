#ifndef MANAGER_HPP
#define MANAGER_HPP
#include "Game/Character.hpp"
#include "Game/Enemy/Enemy.hpp"
#include "Game/Map.hpp"
#include "Game/Passive/Passive.hpp"
#include "Game/Projectile/Projectile.hpp"
#include "Game/Util/QuadTree.hpp"
#include "Game/Util/Timer.hpp"
#include "Game/Weapon/Weapon.hpp"
#include "Util/Text.hpp"
#include <memory>
#include <string>

namespace Game {
class Manager {
public:
    void Start();
    void Update();
    void Draw();
    bool Have(std::string name);
    void EnemyGen();
    void HurtEnemy();
    void AcquireWeapon(std::string name);
    void AddProjectile(std::shared_ptr<Projectile::Projectile> projectile);

private:
    std::set<std::string> m_Have;
    std::set<std::shared_ptr<Weapon::Weapon>> m_Weapons;
    std::set<std::shared_ptr<Passive::Passive>> m_Passives;
    std::set<std::shared_ptr<Projectile::Projectile>> m_Projectiles;
    std::set<std::shared_ptr<Enemy::Enemy>> m_Enemies;
    std::shared_ptr<Character> m_Character;
    std::shared_ptr<Map> m_Map;
    std::shared_ptr<::Util::Text> m_FPS;
    std::shared_ptr<::Util::Text> m_ChrPos;
    std::shared_ptr<Util::QuadTree> m_Plain;
};
extern Manager CAT;
} // namespace Game

#endif
