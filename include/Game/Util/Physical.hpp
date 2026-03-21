#ifndef PHYSICAL_HPP
#define PHYSICAL_HPP

#include "Util.hpp"
#include "pch.hpp"
#include <typeinfo>

namespace Game::Util {
class Physical {
public:
    Physical();
    virtual ~Physical() = default;
    virtual float_t Width() const = 0;
    virtual float_t Height() const = 0;
    virtual float_t Rotation() const = 0;
    virtual const glm::vec2 &GetPosition() const;
    virtual const glm::vec2 &GetVelocity() const;
    const std::string &Type() const;
    virtual float Distance(const std::shared_ptr<Physical> &other);
    virtual bool IsCollideWith(const std::shared_ptr<Physical> &other,
                               bool isRotate = false);
    virtual void SetPosition(glm::vec2 position);
    virtual void SetVelocity(glm::vec2 velocity);

protected:
    std::vector<glm::vec2> getCorners();
    std::vector<glm::vec2> getAxes();
    std::pair<float_t, float_t> project(const std::vector<glm::vec2> &corners,
                                        const glm::vec2 &axis) const;
    bool overlapOnAxis(const std::shared_ptr<Physical> &other,
                       const glm::vec2 &axis);

    glm::vec2 m_Position;
    glm::vec2 m_Velocity;
    std::vector<glm::vec2> m_Corners;
    int32_t m_Epoch;
    std::string m_Type;
    std::vector<glm::vec2> m_Axes;
    mutable std::unordered_map<std::string,
                               std::pair<int, std::vector<glm::vec2>>>
        m_Cache;
};
} // namespace Game::Util

#endif /* PHYSICAL_HPP */
