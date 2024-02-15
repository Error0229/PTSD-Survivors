#include "Game/Projectile/Whip.hpp"
#include "Game/Camera.hpp"
#include "Game/Config.hpp"
#include "Game/Projectile/Projectile.hpp"
#include "Game/Resource.hpp"
#include "Game/Util/Animated.hpp"
#include "Game/Util/Timer.hpp"

namespace Game::Projectile {
Whip::Whip() {
    m_DefaultDirection = Util::Direction::RIGHT;
    m_ID = "WHIP";
    Load("Default", Resource::GetAnimation("WHIP"));
}
void Whip::Start() {
    Util::Animated::Play();
    Projectile::Start();
}
void Whip::Update(const ::Util::Transform &transform) {
    Projectile::Update(transform);
    if (!m_IsStarted)
        return;
    m_Position = Camera::s_Position + m_Offset;
    Util::Animated::Update();
    if (!IsAnimated()) {
        m_IsOver = true;
    }
}
} // namespace Game::Projectile
