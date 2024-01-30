#include "Game/QuadTree.hpp"
#include "Game/Util.hpp"
#include "config.hpp"
#include "pch.hpp"
#include <memory>

namespace Game {

QuadTree::QuadTree() = default;
QuadTree::~QuadTree() {
    Clear();
}
QuadTree::QuadTree(int x, int y, int w, int h, int max_objects, int max_levels,
                   int level)
    : x(x),
      y(y),
      w(w),
      h(h),
      max_objects(max_objects),
      max_levels(max_levels),
      level(level) {
    for (int i = 0; i < 4; i++) {
        children[i] = nullptr;
    }
}
void QuadTree::SetRange(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}
void QuadTree::Insert(std::shared_ptr<Physical> obj) {
    if (obj->GetPosition().x + obj->Width() < x ||
        obj->GetPosition().x > x + w ||
        obj->GetPosition().y + obj->Height() < y ||
        obj->GetPosition().y > y + h)
        return;
    auto objects_size = static_cast<int>(objects.size());
    if (objects_size < max_objects || level == max_levels) {
        objects.push_back(obj);
        return;
    } else {
        if (children[0] == nullptr)
            Split();
        int index = Quadrant(obj);
        if (index != -1) {
            children[index]->Insert(obj);
            return;
        } else {
            objects.push_back(obj);
            return;
        }
    }
}
int QuadTree::Quadrant(std::shared_ptr<Physical> obj) {
    int vertical_midpoint = x + (w >> 1);
    int horizontal_midpoint = y + (h >> 1);
    bool top_quadrant =
        (obj->GetPosition().y + obj->Height() < horizontal_midpoint &&
         obj->GetPosition().y > y);
    bool bottom_quadrant = (obj->GetPosition().y > horizontal_midpoint &&
                            obj->GetPosition().y + obj->Height() < y + h);
    if (obj->GetPosition().x + obj->Width() < vertical_midpoint &&
        obj->GetPosition().x > x) {
        if (top_quadrant) {
            return 0;
        } else if (bottom_quadrant) {
            return 2;
        }
    } else if (obj->GetPosition().x > vertical_midpoint &&
               obj->GetPosition().x + obj->Width() < x + w) {
        if (top_quadrant) {
            return 1;
        } else if (bottom_quadrant) {
            return 3;
        }
    }
    return -1;
}
void QuadTree::Split() {
    int subWidth = (w >> 1);
    int subHeight = (h >> 1);
    children[0] = std::make_unique<QuadTree>(
        x, y, subWidth, subHeight, max_objects, max_levels, level + 1);
    children[1] =
        std::make_unique<QuadTree>(x + subWidth, y, subWidth, subHeight,
                                   max_objects, max_levels, level + 1);
    children[2] =
        std::make_unique<QuadTree>(x, y + subHeight, subWidth, subHeight,
                                   max_objects, max_levels, level + 1);
    children[3] = std::make_unique<QuadTree>(x + subWidth, y + subHeight,
                                             subWidth, subHeight, max_objects,
                                             max_levels, level + 1);
}
void QuadTree::QueryCollision(std::vector<std::shared_ptr<Physical>> &result,
                              std::shared_ptr<Physical> q) {
    int index = Quadrant(q);
    if (index != -1 && children[0] != nullptr) {
        children[index]->QueryCollision(result, q);
    } else {
        if (children[0] != nullptr) {

            for (int i = 0; i < 4; i++) {
                if (IsOverlapped(children[i]->x, children[i]->y, children[i]->w,
                                 children[i]->h, q->GetPosition().x,
                                 q->GetPosition().y, q->Width(), q->Height())) {
                    children[i]->QueryCollision(result, q);
                }
            }
        }
    }
    for (std::shared_ptr<Physical> obj : objects) {
        if (q->IsCollideWith(obj)) {
            result.push_back(obj);
        }
    }
}
void QuadTree::QueryCollision(std::vector<std::shared_ptr<Physical>> &result,
                              std::shared_ptr<Physical> q,
                              const std::type_info &type) {
    int index = Quadrant(q);
    if (index != -1 && children[0] != nullptr) {
        children[index]->QueryCollision(result, q, type);
    } else {
        if (children[0] != nullptr) {
            for (int i = 0; i < 4; i++) {
                if (IsOverlapped(children[i]->x, children[i]->y, children[i]->w,
                                 children[i]->h, q->GetPosition().x,
                                 q->GetPosition().y, q->Width(), q->Height())) {
                    children[i]->QueryCollision(result, q, type);
                }
            }
        }
    }
    for (std::shared_ptr<Physical> obj : objects) {
        if (obj->Type() != type)
            continue;
        if (q->IsCollideWith(obj)) {
            result.push_back(obj);
        }
    }
}
void QuadTree::QueryNearest(std::shared_ptr<Physical> q,
                            const std::type_info &type, glm::vec2 &result,
                            int &min_distance) {
    if (children[0] != nullptr) {
        for (int i = 0; i < 4; i++) {
            children[i]->QueryNearest(q, type, result, min_distance);
        }
    }
    for (std::shared_ptr<Physical> obj : objects) {
        if (obj->Type() != type)
            continue;
        auto dis = q->Distance(obj);
        if (dis < min_distance) {
            min_distance = dis;
            result = obj->GetPosition();
        }
    }
}
void QuadTree::Clear() {
    objects.clear();
    for (int i = 0; i < 4; i++) {
        if (children[i] != nullptr) {
            children[i]->Clear();
            children[i].reset();
        }
    }
}
QuadTree QuadTree::Plain =
    QuadTree(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, QUADTREE_MAX_OBJECTS,
             QUADTREE_MAX_LEVELS, 0);
} // namespace Game
