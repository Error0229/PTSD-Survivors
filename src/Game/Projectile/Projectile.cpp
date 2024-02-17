#include "Game/Projectile/Projectile.hpp"
#include "Game/Camera.hpp"
#include "Game/Config.hpp"
#include "Game/Util/Animated.hpp"
#include "Game/Util/Timer.hpp"
#include "Util/Logger.hpp"
#include <cstdlib>
namespace Game::Projectile {
Projectile::Projectile(std::string ID) {
    m_ID = ID;
}
void Projectile::Start() {
    m_Type = PROJECTILE;
    SetFrameTime(m_["repeatInterval"]);
    m_StartTime = Util::Clock.Now();
    m_IsStarted = true;
    Util::Animated::Play();
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
    m_Delay = 0;
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
    if (!m_IsStarted)
        return;
    if ((IsMirrored() && m_Transform.scale.x > 0) ||
        (!IsMirrored() && m_Transform.scale.x < 0)) {
        m_Transform.scale.x *= -1;
    }
    m_Transform.translation = Camera::WorldToScreen(m_Position);
    Animated::Draw(m_Transform, m_ZIndex);
}
void Projectile::CollideWith(std::shared_ptr<::Util::GameObject> &other) {
    // TODO
}
std::string_view Projectile::ID() const {
    return m_ID;
}
float_t Projectile::Get(const std::string &name) const {
    return m_.at(name);
}
void Projectile::Set(const std::string &name, float_t value) {
    m_[name] = value;
}
} // namespace Game::Projectile
