#include "Game/Resource.hpp"
#include <memory>

namespace Game {
std::map<std::string, std::shared_ptr<Character>> Resource::m_Character;
std::map<Weapon::Type, std::shared_ptr<Weapon::Weapon>> Resource::m_Weapon;
std::map<Weapon::Type, std::shared_ptr<Projectile::Projectile>>
    Resource::m_Projectile;
std::map<std::string, std::shared_ptr<Util::Animation>> Resource::m_Animation;
std::map<std::string, std::shared_ptr<Enemy::Enemy>> Resource::m_Enemy;

void Resource::Initialize() {
    // TODO: load all resources
}

std::shared_ptr<Character> Resource::Character(std::string name) {
    if (m_Character.find(name) == m_Character.end()) {
        throw std::logic_error("Character not found");
    }
    return m_Character[name];
}

std::shared_ptr<Weapon::Weapon> Resource::Weapon(Weapon::Type type) {
    if (m_Weapon.find(type) == m_Weapon.end()) {
        throw std::logic_error("Weapon not found");
    }
    return m_Weapon[type];
}

const std::shared_ptr<Projectile::Projectile>
Resource::Projectile(Weapon::Type type) {
    if (m_Projectile.find(type) == m_Projectile.end()) {
        throw std::logic_error("Projectile not found");
    }
    return m_Projectile[type];
}

std::shared_ptr<Util::Animation> Resource::Animation(std::string name) {
    if (m_Animation.find(name) == m_Animation.end()) {
        throw std::logic_error("Animation not found");
    }
    return m_Animation[name];
}

std::shared_ptr<Enemy::Enemy> Resource::Enemy(std::string name) {
    if (m_Enemy.find(name) == m_Enemy.end()) {
        throw std::logic_error("Enemy not found");
    }
    return m_Enemy[name];
}

} // namespace Game
