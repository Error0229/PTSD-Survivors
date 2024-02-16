#include "Game/Projectile/Vampirica.hpp"
#include "Game/Resource.hpp"
namespace Game::Projectile {
Vampirica::Vampirica() {
    m_ID = "VAMPIRICA";
    Load("Default", Resource::GetAnimation("VAMPIRICA"));
}
} // namespace Game::Projectile
