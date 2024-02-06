#include "Game/Manager.hpp"
#include "Game/Camera.hpp"
#include "Game/Resource.hpp"
#include "Game/Util/Timer.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/Transform.hpp"
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
    for (auto &enemy : m_Enemies) {
        enemy->Update();
    }
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
    LOG_DEBUG("fps: {}", 1 / ::Util::Time::GetDeltaTime());
    // m_FPS->Draw({{-280, 275}, 0, {1, 1}}, 3);
}
bool Manager::Have(std::string name) {
    return m_Have.count(name) > 0;
}
} // namespace Game
