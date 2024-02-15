#include "Game/Projectile/Projectile.hpp"
#include "Game/Util/Timer.hpp"
#include <cstdlib>
namespace Game::Projectile {
Projectile::Projectile(std::string ID) {
    m_ID = ID;
}
void Projectile::Start() {
    m_StartTime = Util::Clock.Now();
    m_IsStarted = true;
}
void Projectile::Update(const ::Util::Transform &transform) {
    if (!m_IsStarted && Util::Clock.Now() - m_CreateTime >= m_Delay) {
        Start();
    }
}
bool Projectile::operator<(const Projectile &rhs) const {
    return this->m_ID < rhs.m_ID;
}
void Projectile::SetUp(std::unordered_map<std::string, float_t> stats) {
    m_ = stats;
    m_CreateTime = Util::Clock.Now();
}
void Projectile::SetDelay(time_t delay) {
    m_Delay = delay;
}
float_t Projectile::Height() const {
    return Util::Animated::Height() * std::abs(m_Transform.scale.x);
}

float_t Projectile::Width() const {
    return Util::Animated::Width() * std::abs(m_Transform.scale.x);
}
float_t Projectile::Rotation() const {
    return m_Transform.rotation;
}
bool Projectile::IsOver() const {
    return m_IsOver;
}
void Projectile::Draw() {
    if ((IsMirrored() && m_Transform.scale.x > 0) ||
        (!IsMirrored() && m_Transform.scale.x < 0)) {
        m_Transform.scale.x *= -1;
    }
    Animated::Draw(m_Transform, m_ZIndex);
}
void Projectile::CollideWith(std::shared_ptr<::Util::GameObject> &other) {
    // TODO
}
} // namespace Game::Projectile
