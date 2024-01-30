#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "Physical.hpp"
#include "config.hpp"
#include "pch.hpp"
#include <typeinfo>

namespace Game {
class QuadTree {
public:
    QuadTree();
    ~QuadTree();
    QuadTree(int x, int y, int w, int h, int max_objects, int max_levels,
             int level);
    void SetRange(int, int, int, int);
    void Insert(std::shared_ptr<Physical> object);
    void Clear();
    void QueryCollision(std::vector<std::shared_ptr<Physical>> &result,
                        std::shared_ptr<Physical> object);
    void QueryCollision(std::vector<std::shared_ptr<Physical>> &result,
                        std::shared_ptr<Physical> object,
                        const std::type_info &type);
    void QueryNearest(std::shared_ptr<Physical> object,
                      const std::type_info &type, glm::vec2 &result,
                      int &min_distance);
    static QuadTree Plain;

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
} // namespace Game
#endif /* QUADTREE_HPP */
