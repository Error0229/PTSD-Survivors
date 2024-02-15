#ifndef OFFSETTED_HPP
#define OFFSETTED_HPP
#include "pch.hpp"
namespace Game::Util {
class Offsetted {
public:
    void SetOffset(const glm::vec2 &offset);
    const glm::vec2 &GetOffset() const;

protected:
    glm::vec2 m_Offset;
};
} // namespace Game::Util
#endif /* OFFSETTED_HPP */
