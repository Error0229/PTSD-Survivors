#ifndef RESOURCE_HPP
#define RESOURCE_HPP
#include "Game/Character.hpp"
#include "Game/Enemy/Enemy.hpp"
#include "Game/Passive/Passive.hpp"
#include "Game/Projectile/Projectile.hpp"
#include "Game/Util/Animation.hpp"
#include "Game/Util/ObjectPool.hpp"
#include "Game/Util/ObjectPool.inl"
#include "Game/Weapon/Weapon.hpp"
#include "pch.hpp"
namespace Game {
class Resource {
public:
    static void Initialize();
    static std::shared_ptr<Character> GetCharacter(std::string name);
    static std::shared_ptr<Weapon::Weapon> GetWeapon(std::string type);
    static std::shared_ptr<Passive::Passive> GetPassive(std::string type);
    static std::shared_ptr<Projectile::Projectile>
    GetProjectile(std::string type);
    static void
    ReturnProjectile(std::string type,
                     std::shared_ptr<Projectile::Projectile> projectile);
    static std::shared_ptr<Util::Animation> GetAnimation(std::string name);
    static std::shared_ptr<Enemy::Enemy> GetEnemy(std::string name);
    static void ReturnEnemy(std::string name,
                            std::shared_ptr<Enemy::Enemy> enemy);
    static std::vector<std::string> s_NormalEnemies;
    static std::vector<std::string> s_BossEnemies;
    static std::vector<std::string> s_SwarmEnemies;
    static std::vector<std::string> s_Characters;
    static std::vector<std::string> s_Weapons;
    static std::vector<std::string> s_Passives;
    static std::vector<std::string> s_Projectiles;
    static bool HaveAnimation(std::string name);

private:
    static std::unordered_map<std::string, std::shared_ptr<Game::Character>>
        s_Character;
    static std::unordered_map<std::string, std::shared_ptr<Weapon::Weapon>>
        s_Weapon;
    static std::unordered_map<std::string,
                              std::shared_ptr<Game::Passive::Passive>>
        s_Passive;
    static std::unordered_map<std::string,
                              std::shared_ptr<Projectile::Projectile>>
        s_Projectile;
    static std::unordered_map<std::string,
                              Util::ObjectPool<Projectile::Projectile>>
        s_ProjectilePool;
    static std::unordered_map<std::string, std::shared_ptr<Util::Animation>>
        s_Animation;
    static std::unordered_map<std::string, std::shared_ptr<Enemy::Enemy>>
        s_Enemy;
    static std::unordered_map<std::string, Util::ObjectPool<Enemy::Enemy>>
        s_EnemyPool;
};
} // namespace Game

#endif // RESOURCE_HPP
