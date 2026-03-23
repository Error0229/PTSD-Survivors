#ifndef MANAGER_HPP
#define MANAGER_HPP
#include "Core/SpriteBatch.hpp"
#include "Game/Character.hpp"
#include "Game/Enemy/Enemy.hpp"
#include "Game/LevelUpSystem.hpp"
#include "Game/Map.hpp"
#include "Game/Passive/Passive.hpp"
#include "Game/Pickup.hpp"
#include "Game/Projectile/Projectile.hpp"
#include "Game/Util/QuadTree.hpp"
#include "Game/Util/Timer.hpp"
#include "Game/WaveManager.hpp"
#include "Game/Weapon/Weapon.hpp"
#include "Util/Text.hpp"
#include <memory>
#include <string>

namespace Game {

enum class GameState {
    PLAYING,
    PAUSED,
    LEVEL_UP,
    CHEST_OPEN,
    GAME_OVER,
};

class Manager {
public:
    void Start();
    void Update();
    void Draw();
    bool Have(std::string name);
    void EnemyGen();
    void HurtEnemy();
    void AcquireWeapon(std::string name);
    void AcquirePassive(std::string name);
    void AddProjectile(std::shared_ptr<Projectile::Projectile> projectile);
    bool TryEvolveWeapon(); // returns true if a weapon was evolved

    GameState GetState() const { return m_State; }
    float GetGameTime() const { return m_GameTime; }

    // Query helpers for behaviors
    std::shared_ptr<Character> GetCharacter() const { return m_Character; }
    const std::set<std::shared_ptr<Enemy::Enemy>> &GetEnemies() const {
        return m_Enemies;
    }

private:
    GameState m_State = GameState::PLAYING;
    float m_GameTime = 0.0f; // seconds elapsed since game start

    std::set<std::string> m_Have;
    std::set<std::shared_ptr<Weapon::Weapon>> m_Weapons;
    std::set<std::shared_ptr<Passive::Passive>> m_Passives;
    std::set<std::shared_ptr<Projectile::Projectile>> m_Projectiles;
    std::set<std::shared_ptr<Enemy::Enemy>> m_Enemies;
    std::shared_ptr<Character> m_Character;
    std::shared_ptr<Map> m_Map;
    std::shared_ptr<::Util::Text> m_FPS;
    std::shared_ptr<::Util::Text> m_ChrPos;
    std::vector<Pickup> m_Pickups; // flat list, not in QuadTree
    std::shared_ptr<Util::QuadTree> m_Plain;
    std::unique_ptr<Core::SpriteBatch> m_Batch;
    WaveManager m_WaveManager;
    int m_PendingLevelUps = 0; // queued level-ups from multi-level XP events
    int m_LastVacuumMinute = -1;
};
extern Manager CAT;
} // namespace Game

#endif
