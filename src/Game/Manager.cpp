#include "Game/Manager.hpp"
#include "Game/Camera.hpp"
#include "Game/Resource.hpp"
#include "Game/Util/Timer.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/Transform.hpp"
#include "pch.hpp"
#include <string>
namespace Game {
Manager CAT;
void Manager::Start() {
    Resource::Initialize();
    m_Character = Resource::GetCharacter("TATANKA");
    m_Character->SetAnimation("Default");
    m_Character->Start();
    m_Map = std::make_shared<Map>();
    m_Map->Setup("map");
    m_Map->Start();
    m_FPS = std::make_shared<::Util::Text>("../resources/Font/ANY.ttf", 24,
                                           "FPS: 0");
    Util::Clock.Start();
}
void Manager::Update() {
    m_Character->Update({::Util::Input::GetCursorPosition()});
    m_Map->Update({m_Character->GetPosition()});
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
    toErase.clear();
    for (auto &projectile : m_Projectiles) {
        projectile->Update();
    }
    // m_FPS = std::make_shared<::Util::Text>(
    //     "../resources/Font/ANY.ttf", 24,
    //     "FPS: " + std::to_string(1 / ::Util::Time::GetDeltaTime()));
}
void Manager::Draw() {
    m_Map->Draw();
    m_Character->Draw();
    for (auto &enemy : m_Enemies) {
        enemy->Draw();
    }
    for (auto &projectile : m_Projectiles) {
        projectile->Draw();
    }
    // LOG_DEBUG("fps: {}", 1 / ::Util::Time::GetDeltaTime());
    // m_FPS->Draw({{-280, 275}, 0, {1, 1}}, 3);
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
    static int32_t i = 0;
    auto &enemyName = Resource::s_NormalEnemies[i++];
    auto enemy = Resource::GetEnemy(enemyName);
    enemy->Start();
    m_Enemies.insert(enemy);
}
void Manager::HurtEnemy() {
    for (auto &enemy : m_Enemies) {
        enemy->Hurt(100);
    }
}
} // namespace Game
