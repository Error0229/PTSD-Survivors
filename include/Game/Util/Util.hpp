#ifndef UTIL_HPP
#define UTIL_HPP
#include "pch.hpp"
namespace Game::Util {
bool IsOverlapped(int x1, int y1, int w1, int h1, int x2, int y2, int w2,
                  int h2);
enum class Direction { UP, DOWN, LEFT, RIGHT };

struct Size {
    float w;
    float h;
};
const float_t PI = std::acos(-1);
} // namespace Game::Util

#endif /* UTIL_HPP */
