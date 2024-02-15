#ifndef MIRRORED_HPP
#define MIRRORED_HPP
#include "Game/Util/Util.hpp"
#include "pch.hpp"
namespace Game::Util {
class Mirrored {
public:
    bool IsMirrored() const;
    void SetDirection(Direction direction);
    Direction GetDirection() const;

protected:
    Direction m_DefaultDirection, m_Direction = Direction::RIGHT;
};
} // namespace Game::Util

#endif /* MIRRORED_HPP */
