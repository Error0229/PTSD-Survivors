#ifndef ENEMY_HPP
#define ENEMY_HPP
#include "Game/Util/Physical.hpp"
#include "Util/GameObject.hpp"

namespace Game::Enemy {
class Enemy : public ::Util::GameObject, public Util::Physical {};

} // namespace Game::Enemy

#endif
