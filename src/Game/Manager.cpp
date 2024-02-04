#include "Game/Manager.hpp"
#include "Game/Camera.hpp"
#include "Game/Resource.hpp"
#include "Game/Util/Timer.hpp"
#include "Util/Input.hpp"
namespace Game {
Manager 🧝;
void Manager::Start() {
    Resource::Initialize();
    m_Character = Resource::GetCharacter("TATANKA");
    m_Character->SetAnimation("Default");
    m_Character->Start();
    m_Map = std::make_shared<Map>();
    m_Map->Setup("map");
    m_Map->Start();
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
}
bool Manager::Have(std::string name) {
    return m_Have.count(name) > 0;
}
} // namespace Game
