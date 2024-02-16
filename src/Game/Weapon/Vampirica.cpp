#include "Game/Weapon/Vampirica.hpp"
#include "Game/Camera.hpp"
#include "Game/Manager.hpp"
#include "Game/Projectile/Vampirica.hpp"
#include "Game/Util/Timer.hpp"
#include "Util/Input.hpp"
namespace Game::Weapon {
void Vampirica::Update(const ::Util::Transform &transform) {
    if (Util::Clock.Now() - m_LastTimeAttack < m_["interval"]) {
        return;
    }
    m_LastTimeAttack = Util::Clock.Now();
    auto mouse = Camera::ScreenToWorld(::Util::Input::GetCursorPosition());
    auto chrPos = Camera::s_Position;
    for (int32_t i = 0; i < static_cast<int32_t>(m_["amount"]); i++) {
        auto proj = std::make_shared<Game::Projectile::Vampirica>();
        proj->SetUp(m_);
        if (mouse.x < chrPos.x) {
            if (!(i & 1)) {
                proj->SetOffset({-proj->Width() / 2, proj->Height() / 2 * i});
                proj->SetDirection(Util::Direction::LEFT);
                proj->SetDelay(i * 2 * m_["repeatInterval"]);
            } else {
                proj->SetOffset(
                    {proj->Width() / 2, -proj->Height() / 2 * (i - 1)});
                proj->SetDirection(Util::Direction::RIGHT);
                proj->SetDelay((i - 1) * 2 * m_["repeatInterval"]);
                proj->Flip();
            }
        } else {
            if (!(i & 1)) {
                proj->SetOffset({proj->Width() / 2, proj->Height() / 2 * i});
                proj->SetDirection(Util::Direction::RIGHT);
                proj->SetDelay(i * 2 * m_["repeatInterval"]);
            } else {
                proj->SetOffset(
                    {-proj->Width() / 2, -proj->Height() / 2 * (i - 1)});
                proj->SetDirection(Util::Direction::LEFT);
                proj->SetDelay((i - 1) * 2 * m_["repeatInterval"]);
                proj->Flip();
            }
        }
        CAT.AddProjectile(proj);
    }
}
} // namespace Game::Weapon
