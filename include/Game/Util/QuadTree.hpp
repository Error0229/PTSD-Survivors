#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "Game/Util/Physical.hpp"
#include "config.hpp"
#include "pch.hpp"
#include <typeinfo>

namespace Game::Util {
class QuadTree {
public:
    QuadTree();
    ~QuadTree();
    QuadTree(int x, int y, int w, int h, int maxObjects, int maxLevels,
             int level);
    void SetRange(int, int, int, int);
    void Insert(std::shared_ptr<Physical> object);
    void Clear();
    void QueryCollision(std::shared_ptr<Physical> object,
                        std::vector<std::shared_ptr<Physical>> &result);
    void QueryCollision(std::shared_ptr<Physical> object,
                        const std::type_info &type,
                        std::vector<std::shared_ptr<Physical>> &result);
    void QueryNearest(std::shared_ptr<Physical> object,
                      const std::type_info &type, glm::vec2 &result,
                      int &distance);
    static QuadTree s_Plain;

private:
    std::vector<std::shared_ptr<Physical>> objects;
    std::array<std::unique_ptr<QuadTree>, 4> children;
    int x, y, w, h;
    int max_objects;
    int max_levels;
    int level;
    void Split();
    int Quadrant(std::shared_ptr<Physical> object);
};
} // namespace Game::Util
#endif /* QUADTREE_HPP */
