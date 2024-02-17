#include "Game/Manager.hpp"
#include "Game/Camera.hpp"
#include "Game/Config.hpp"
#include "Game/Projectile/Projectile.hpp"
#include "Game/Resource.hpp"
#include "Game/Util/Physical.hpp"
#include "Game/Util/Timer.hpp"
#include "Game/Weapon/Vampirica.hpp"
#include "Game/Weapon/Whip.hpp"
#include "Util/Color.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/Transform.hpp"
#include "config.hpp"
#include "pch.hpp"
#include <execution>
#include <memory>
namespace Game {
Manager CAT;
void Manager::Start() {
    Resource::Initialize();
    m_Character = Resource::GetCharacter("TATANKA");
    m_Character->SetAnimation("Default");
    m_Character->Start();
    m_Map = std::make_shared<Map>();
    m_Map->Setup("dummy5");
    m_Map->Start();
    m_FPS = std::make_shared<::Util::Text>(
        "../resources/Font/ANY.ttf", 24, "FPS: 0",
        ::Util::Color{135 / 255., 206 / 255., 235 / 255., 1});
    m_ChrPos = std::make_shared<::Util::Text>(
        "../resources/Font/ANY.ttf", 24, "ChrPos: 0",
        ::Util::Color{135 / 255., 206 / 255., 235 / 255., 1});
    m_Plain = std::make_shared<Util::QuadTree>(
        0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, QUADTREE_MAX_OBJECTS,
        QUADTREE_MAX_LEVELS, 0);
    AcquireWeapon("VAMPIRICA");
    Util::Clock.Start();
}
void Manager::Update() {
    m_Character->Update({::Util::Input::GetCursorPosition()});
    m_Map->Update({m_Character->GetPosition()});
    std::for_each(std::execution::par, m_Weapons.begin(), m_Weapons.end(),
                  [](auto &weapon) { weapon->Update(); });
    std::erase_if(m_Projectiles, [](auto &projectile) {
        projectile->Update();
        return projectile->IsOver();
    });

    // handle projectile collision
    m_Plain->Clear();
    m_Plain->SetRange(m_Character->GetPosition().x - 1000,
                      m_Character->GetPosition().y - 1000, 2000, 2000);
    for (auto &projectile : m_Projectiles) {
        m_Plain->Insert(projectile);
    }
    for (auto &enemy : m_Enemies) {
        m_Plain->Insert(enemy);
    }
    std::vector<std::shared_ptr<Util::Physical>> result;
    for (auto &enemy : m_Enemies) {
        m_Plain->QueryCollision(enemy, PROJECTILE, result);
        for (auto &other : result) {
            if (enemy->IsCollideWith(other)) {
                enemy->CollisionWith(
                    std::static_pointer_cast<Projectile::Projectile>(other));
            }
        }
        result.clear();
    }
    static std::vector<std::shared_ptr<Enemy::Enemy>> toErase;
    for (auto &enemy : m_Enemies) {
        enemy->Update({m_Character->GetPosition()});
        if (enemy->IsOver()) {
            toErase.push_back(enemy);
        }
    }
    for (auto &enemy : toErase) {
        Resource::ReturnEnemy(enemy->ID(), enemy);
        m_Enemies.erase(enemy);
    }
    // handle enemy collision
    result.clear();
    m_Plain->Clear();
    for (auto &enemy : m_Enemies) {
        m_Plain->Insert(enemy);
    }
    for (auto &enemy : m_Enemies) {
        m_Plain->QueryCollision(enemy, ENEMY, result);
        for (auto &other : result) {
            if (enemy->IsCollideWith(other)) {
                enemy->CollisionWith(
                    std::static_pointer_cast<Enemy::Enemy>(other));
            }
        }
        if (enemy->IsCollideWith(
                std::static_pointer_cast<Game::Util::Physical>(m_Character))) {
            enemy->CollisionWith(m_Character);
        }
        result.clear();
    }

    toErase.clear();
    m_FPS->SetText("FPS: " + std::to_string(1 / ::Util::Time::GetDeltaTime()));
    m_ChrPos->SetText("ChrPos:" + glm::to_string(m_Character->GetPosition()));
}
void Manager::Draw() {
    m_Map->Draw();
    m_Character->Draw();
    std::for_each(std::execution::par, m_Enemies.begin(), m_Enemies.end(),
                  [](auto &enemy) { enemy->Draw(); });
    for (auto &projectile : m_Projectiles) {
        projectile->Draw();
    }
    m_FPS->Draw({{-280, 275}, 0, {1, 1}}, 3);
    m_ChrPos->Draw({{-280, 250}, 0, {1, 1}}, 3);
}
bool Manager::Have(std::string name) {
    return m_Have.count(name) > 0;
}

void Manager::EnemyGen() { /*
     static std::random_device rd;
     static std::mt19937 gen(rd());
     std::uniform_int_distribution<> distribute(0,
     Resource::s_NormalEnemies.size() - 1); auto enemyName =
     std::next(Resource::s_NormalEnemies.begin(), distribute(gen));*/
    static size_t i = 0;
    static std::string enemyName;
    if (i < Resource::s_NormalEnemies.size()) {
        enemyName = Resource::s_NormalEnemies[i++];
    } else {
        enemyName =
            Resource::s_BossEnemies[i - Resource::s_NormalEnemies.size()];
        i++;
    }
    auto enemy = Resource::GetEnemy(enemyName);
    enemy->Start();
    m_Enemies.insert(enemy);
}
void Manager::HurtEnemy() {
    for (auto &enemy : m_Enemies) {
        enemy->Hurt(100);
    }
}
void Manager::AcquireWeapon(std::string name) {
    m_Have.insert(name);
    auto weapon = Resource::GetWeapon(name);
    if (name == "WHIP") {
        weapon = std::static_pointer_cast<Weapon::Weapon>(
            std::make_shared<Weapon::Whip>(*weapon));
    } else if (name == "VAMPIRICA") {
        weapon = std::static_pointer_cast<Weapon::Weapon>(
            std::make_shared<Weapon::Vampirica>(*weapon));
    }
    weapon->Start();
    m_Weapons.insert(weapon);
}
void Manager::AddProjectile(
    std::shared_ptr<Projectile::Projectile> projectile) {
    m_Projectiles.insert(projectile);
}
} // namespace Game
