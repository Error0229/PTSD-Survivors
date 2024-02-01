#include "Game/Manager.hpp"
#include "Game/Camera.hpp"
#include "Game/Resource.hpp"
#include "Util/Input.hpp"
namespace Game {
void Manager::Start() {
    Resource::Initialize();
    m_Character = std::make_shared<Character>("cat");
    m_Map = std::make_shared<Map>("map");
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
} // namespace Game
