#ifndef RESOURCE_HPP
#define RESOURCE_HPP
#include "Game/Character.hpp"
#include "Game/Enemy/Enemy.hpp"
#include "Game/Projectile/Projectile.hpp"
#include "Game/Util/Animation.hpp"
#include "Game/Util/ObjectPool.hpp"
#include "Game/Util/ObjectPool.inl"
#include "Game/Weapon/Weapon.hpp"
namespace Game {
class Resource {
public:
    static void Initialize();
    static std::shared_ptr<Character> GetCharacter(std::string name);
    static std::shared_ptr<Weapon::Weapon> GetWeapon(std::string type);
    static std::shared_ptr<Projectile::Projectile>
    GetProjectile(std::string type);
    static void
    ReturnProjectile(std::string type,
                     std::shared_ptr<Projectile::Projectile> projectile);
    static std::shared_ptr<Util::Animation> GetAnimation(std::string name);
    static std::shared_ptr<Enemy::Enemy> GetEnemy(std::string name);
    static void ReturnEnemy(std::string name,
                            std::shared_ptr<Enemy::Enemy> enemy);

private:
    static std::map<std::string, std::shared_ptr<Game::Character>> s_Character;
    static std::map<std::string, std::shared_ptr<Weapon::Weapon>> s_Weapon;
    static std::map<std::string, std::shared_ptr<Projectile::Projectile>>
        s_Projectile;
    static std::map<std::string, Util::ObjectPool<Projectile::Projectile>>
        s_ProjectilePool;
    static std::map<std::string, std::shared_ptr<Util::Animation>> s_Animation;
    static std::map<std::string, std::shared_ptr<Enemy::Enemy>> s_Enemy;
    static std::map<std::string, Util::ObjectPool<Enemy::Enemy>> s_EnemyPool;
};
} // namespace Game

#endif // RESOURCE_HPP
