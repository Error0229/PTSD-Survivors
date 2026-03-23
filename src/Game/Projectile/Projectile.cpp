#include "Game/Projectile/Projectile.hpp"
#include "Game/Camera.hpp"
#include "Game/Config.hpp"
#include "Game/Resource.hpp"
#include "Game/Util/Animated.hpp"
#include "Game/Util/Timer.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include <cstdlib>
namespace Game::Projectile {
Projectile::Projectile(std::string ID) {
    m_ID = ID;
}

void Projectile::Start() {
    m_Type = PROJECTILE;
    // Load animation from Resource cache if available
    if (Resource::HaveAnimation(m_ID)) {
        Load("Default", Resource::GetAnimation(m_ID));
        SetAnimation("Default");
    }
    if (m_.count("repeatInterval")) {
        SetFrameTime(m_["repeatInterval"]);
    }
    m_StartTime = Util::Clock.Now();
    m_IsStarted = true;
    Util::Animated::Play();
}

void Projectile::Update(const ::Util::Transform &transform) {
    if (!m_IsStarted && Util::Clock.Now() - m_CreateTime >= m_Delay) {
        Start();
    }
    if (!m_IsStarted)
        return;

    // Delegate movement to behavior if set
    if (m_HasBehavior) {
        float dt = ::Util::Time::GetDeltaTime();
        // playerPos comes from Camera::s_Position as a proxy
        UpdateBehavior(dt, Camera::s_Position);

        // Check duration expiry (skip for permanent behaviors like VESPERS)
        bool isPermanent = false;
        if (auto *orbit = std::get_if<OrbitalCircle>(&m_Behavior)) {
            isPermanent = orbit->permanent;
        }
        if (!isPermanent) {
            m_Duration -= dt;
            if (m_Duration <= 0) {
                m_IsOver = true;
            }
        }
    }

    // Check if animation finished (for non-behavior projectiles)
    if (!m_HasBehavior && m_IsStarted && !IsAnimated()) {
        m_IsOver = true;
    }
}

bool Projectile::operator<(const Projectile &rhs) const {
    return this->m_ID < rhs.m_ID;
}

void Projectile::SetUp(std::unordered_map<std::string, float_t> stats) {
    m_ = stats;
    m_CreateTime = Util::Clock.Now();
    m_Delay = 0;
    m_Duration = stats.count("duration") ? stats.at("duration") / 1000.0f
                                         : 5.0f; // default 5s
}

void Projectile::SetDelay(time_t delay) {
    m_Delay = delay;
}

float_t Projectile::Height() const {
    try {
        return Util::Animated::Height() * std::abs(m_Transform.scale.x);
    } catch (...) {
        return 16.0f; // fallback for weapons without animations
    }
}

float_t Projectile::Width() const {
    try {
        return Util::Animated::Width() * std::abs(m_Transform.scale.x);
    } catch (...) {
        return 16.0f;
    }
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
    // Only draw if animation is loaded (weapons without sprites are logic-only)
    if (Resource::HaveAnimation(m_ID)) {
        Animated::Draw(m_Transform, m_ZIndex);
    }
}

void Projectile::CollideWith(std::shared_ptr<::Util::GameObject> &other) {
    // Collision is handled by Enemy::CollisionWith(Projectile)
}

std::string_view Projectile::ID() const {
    return m_ID;
}

float_t Projectile::Get(const std::string &name) const {
    auto it = m_.find(name);
    if (it != m_.end())
        return it->second;
    return 0.0f;
}

void Projectile::Set(const std::string &name, float_t value) {
    m_[name] = value;
}

// ── Behavior wiring ─────────────────────────────────────

void Projectile::SetBehavior(BehaviorVariant behavior) {
    m_Behavior = std::move(behavior);
    m_HasBehavior = true;
}

void Projectile::StartBehavior(const BehaviorContext &ctx) {
    std::visit([&](auto &b) { b.Start(*this, ctx); }, m_Behavior);
}

void Projectile::UpdateBehavior(float dt, const glm::vec2 &playerPos) {
    std::visit([&](auto &b) { b.Update(*this, dt, playerPos); }, m_Behavior);
}

void Projectile::ResetForReuse() {
    m_IsOver = false;
    m_IsStarted = false;
    m_HasBehavior = false;
    m_Duration = 0;
    m_Delay = 0;
    m_.clear();
    std::visit([](auto &b) { b.Reset(); }, m_Behavior);
    m_Behavior = Unimplemented{};
}

} // namespace Game::Projectile
