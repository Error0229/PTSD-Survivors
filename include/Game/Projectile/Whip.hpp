#ifndef PROJECTILE_WHIP_HPP
#define PROJECTILE_WHIP_HPP

#include "Game/Projectile/Projectile.hpp"
#include "Game/Util/Offsetted.hpp"

namespace Game::Projectile {
class Whip : public Projectile, public Game::Util::Offsetted {
public:
    Whip();
    void Start() override;
    void Update(const ::Util::Transform &transform) override;
};
} // namespace Game::Projectile
#endif /* WHIP_HPP */
