#include "Game/Resource.hpp"
#include <memory>

namespace Game {
std::map<std::string, std::shared_ptr<Character>> Resource::m_Character;
std::map<Weapon::Type, std::shared_ptr<Weapon::Weapon>> Resource::m_Weapon;
std::map<Weapon::Type, std::shared_ptr<Projectile::Projectile>>
    Resource::m_Projectile;
std::map<Weapon::Type, Util::ObjectPool<Projectile::Projectile>>
    Resource::m_ProjectilePool;
std::map<std::string, std::shared_ptr<Util::Animation>> Resource::m_Animation;
std::map<std::string, std::shared_ptr<Enemy::Enemy>> Resource::m_Enemy;
std::map<std::string, Util::ObjectPool<Enemy::Enemy>> Resource::m_EnemyPool;
void Resource::Initialize() {
    // TODO: load all resources
}

std::shared_ptr<Character> Resource::GetCharacter(std::string name) {
    if (m_Character.find(name) == m_Character.end()) {
        throw std::logic_error("Character not found");
    }
    return m_Character[name];
}

std::shared_ptr<Weapon::Weapon> Resource::GetWeapon(Weapon::Type type) {
    if (m_Weapon.find(type) == m_Weapon.end()) {
        throw std::logic_error("Weapon not found");
    }
    return m_Weapon[type];
}

std::shared_ptr<Projectile::Projectile>
Resource::GetProjectile(Weapon::Type type) {
    if (m_Projectile.find(type) == m_Projectile.end()) {
        throw std::logic_error("Projectile not found");
    }
    return m_ProjectilePool[type].GetObject();
}

void Resource::ReturnProjectile(
    Weapon::Type type, std::shared_ptr<Projectile::Projectile> projectile) {
    m_ProjectilePool[type].ReturnObject(projectile);
}

std::shared_ptr<Util::Animation> Resource::GetAnimation(std::string name) {
    if (m_Animation.find(name) == m_Animation.end()) {
        throw std::logic_error("Animation not found");
    }
    return m_Animation[name];
}

std::shared_ptr<Enemy::Enemy> Resource::GetEnemy(std::string name) {
    if (m_Enemy.find(name) == m_Enemy.end()) {
        throw std::logic_error("Enemy not found");
    }
    return m_EnemyPool[name].GetObject();
}

void Resource::ReturnEnemy(std::string name,
                           std::shared_ptr<Enemy::Enemy> enemy) {
    m_EnemyPool[name].ReturnObject(enemy);
}

} // namespace Game
