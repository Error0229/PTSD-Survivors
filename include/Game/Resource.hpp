#ifndef RESOURCE_HPP
#define RESOURCE_HPP
#include "Game/Character.hpp"
#include "Game/Enemy/Enemy.hpp"
#include "Game/Projectile/Projectile.hpp"
#include "Game/Util/Animation.hpp"
#include "Game/Weapon/Weapon.hpp"
namespace Game {
class Resource {
public:
    static void Initialize();
    static std::shared_ptr<Character> Character(std::string name);
    static std::shared_ptr<Weapon::Weapon> Weapon(Weapon::Type type);
    static const std::shared_ptr<Projectile::Projectile>
    Projectile(Weapon::Type type);
    static std::shared_ptr<Util::Animation> Animation(std::string name);
    static std::shared_ptr<Enemy::Enemy> Enemy(std::string name);

private:
    static std::map<std::string, std::shared_ptr<Game::Character>> m_Character;
    static std::map<Weapon::Type, std::shared_ptr<Weapon::Weapon>> m_Weapon;
    static std::map<Weapon::Type, std::shared_ptr<Projectile::Projectile>>
        m_Projectile;
    static std::map<std::string, std::shared_ptr<Util::Animation>> m_Animation;
    static std::map<std::string, std::shared_ptr<Enemy::Enemy>> m_Enemy;
};
} // namespace Game

#endif // RESOURCE_HPP
