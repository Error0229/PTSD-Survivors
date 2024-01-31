#include "Game/Projectile/Projectile.hpp"
#include <cstdlib>

namespace Game::Projectile {
bool Projectile::operator<(const Projectile &rhs) const {
    return this->m_Type < rhs.m_Type;
}
void Projectile::SetStatus(Stats status) {
    m_Stats = status;
}
float_t Projectile::Height() {
    // FIXME: implement with rotation
    return m_Drawable->GetSize().y * m_Transform.scale.y;
}

float_t Projectile::Width() {
    // FIXME: implement with rotation
    return m_Drawable->GetSize().x * m_Transform.scale.x;
}

void Projectile::CollideWith(std::shared_ptr<::Util::GameObject> &other) {
    // TODO
}

void Projectile::Initialize() {
    // TODO: load from file
}

void Projectile::Reset() {
    // TODO
}
std::set<std::shared_ptr<Projectile>> Projectile::s_Projectiles =
    std::set<std::shared_ptr<Projectile>>();
} // namespace Game::Projectile
