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
    Projectile::Start();
    m_Position = Camera::s_Position + m_Offset;
}
void Whip::Update(const ::Util::Transform &transform) {
    Projectile::Update(transform);
    if (!m_IsStarted)
        return;
    m_Position = Camera::s_Position + m_Offset;
    if (!IsAnimated()) {
        m_IsOver = true;
    }
}
void Whip::Flip() {
    m_Transform.scale.y *= -1;
}
} // namespace Game::Projectile
