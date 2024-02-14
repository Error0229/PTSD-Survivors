#ifndef MAP_HPP
#define MAP_HPP

#include "Util/GameObject.hpp"
#include <glm/fwd.hpp>
#include <memory>
namespace Game {
class Map : public ::Util::GameObject {
public:
    Map() = default;
    Map(std::string name);
    ~Map() override = default;
    void Start();
    void Setup(std::string name);
    void Update(const ::Util::Transform &transform = ::Util::Transform());
    void Draw() override;

private:
    float m_Height, m_Width;
    glm::vec2 m_Position;
    std::string m_Name;
    bool m_Enabled = false;
    std::set<std::pair<int32_t, int32_t>> m_VisibleTiles;
    std::deque<std::shared_ptr<Map>> m_Maps;
};
} // namespace Game

#endif /* MAP_HPP */
