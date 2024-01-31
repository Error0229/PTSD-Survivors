#ifndef UTIL_HPP
#define UTIL_HPP
namespace Game::Util {
bool IsOverlapped(int x1, int y1, int w1, int h1, int x2, int y2, int w2,
                  int h2);
enum Direction { UP = 1000, DOWN, LEFT, RIGHT };

struct Size {
    float w;
    float h;
};

} // namespace Game::Util

#endif /* UTIL_HPP */
