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
    static std::shared_ptr<Weapon::Weapon> GetWeapon(Weapon::Type type);
    static std::shared_ptr<Projectile::Projectile>
    GetProjectile(Weapon::Type type);
    static void
    ReturnProjectile(Weapon::Type type,
                     std::shared_ptr<Projectile::Projectile> projectile);
    static std::shared_ptr<Util::Animation> GetAnimation(std::string name);
    static std::shared_ptr<Enemy::Enemy> GetEnemy(std::string name);
    static void ReturnEnemy(std::string name,
                            std::shared_ptr<Enemy::Enemy> enemy);

private:
    static std::map<std::string, std::shared_ptr<Game::Character>> m_Character;
    static std::map<Weapon::Type, std::shared_ptr<Weapon::Weapon>> m_Weapon;
    static std::map<Weapon::Type, std::shared_ptr<Projectile::Projectile>>
        m_Projectile;
    static std::map<Weapon::Type, Util::ObjectPool<Projectile::Projectile>>
        m_ProjectilePool;
    static std::map<std::string, std::shared_ptr<Util::Animation>> m_Animation;
    static std::map<std::string, std::shared_ptr<Enemy::Enemy>> m_Enemy;
    static std::map<std::string, Util::ObjectPool<Enemy::Enemy>> m_EnemyPool;
};
} // namespace Game

#endif // RESOURCE_HPP
