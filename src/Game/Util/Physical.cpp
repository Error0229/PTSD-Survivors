#include "Game/Util/Physical.hpp"
#include "Core/Context.hpp"
namespace Game::Util {

Physical::Physical() {
    m_Position = {0, 0};
    m_Cache["Corners"] = {0, std::vector<glm::vec2>(4)};
    m_Cache["Axes"] = {0, std::vector<glm::vec2>(4)};
}

const glm::vec2 &Physical::GetPosition() const {
    return m_Position;
}

const glm::vec2 &Physical::GetVelocity() const {
    return m_Velocity;
}

const std::string &Physical::Type() const {
    return m_Type;
}

float Physical::Distance(const std::shared_ptr<Physical> &other) {
    return glm::distance(m_Position, other->GetPosition());
}

bool Physical::IsCollideWith(const std::shared_ptr<Physical> &other,
                             bool isRotate) {
    if (!isRotate)
        return IsOverlapped(m_Position.x, m_Position.y, Width(), Height(),
                            other->GetPosition().x, other->GetPosition().y,
                            other->Width(), other->Height());
    auto selfAxes = getAxes();
    auto otherAxes = other->getAxes();
    std::vector<glm::vec2> axes{selfAxes.begin(), selfAxes.end()};
    axes.insert(axes.end(), otherAxes.begin(), otherAxes.end());
    for (const auto &axis : axes) {
        if (!overlapOnAxis(other, axis)) {
            return false; // No collision if there is no overlap on one axis
        }
    }
    return true; // Collision if there is overlap on all axes
}
std::vector<glm::vec2> Physical::getCorners() {
    auto &[epoch, corners] = m_Cache["Corners"];
    if (epoch == Core::Context::Counter) {
        return corners;
    }
    glm::vec2 halfExtents(Width() / 2, Height() / 2);
    // Corners in local space (assuming center at origin)
    std::vector<glm::vec2> localCorners = {{-halfExtents.x, -halfExtents.y},
                                           {halfExtents.x, -halfExtents.y},
                                           {halfExtents.x, halfExtents.y},
                                           {-halfExtents.x, halfExtents.y}};
    const auto rotation = Rotation();
    // Rotate and translate corners
    for (int i = 0; i < 4; ++i) {
        corners[i] = glm::rotate(localCorners[i], rotation) + m_Position;
    }
    epoch = Core::Context::Counter;
    return corners;
}
std::vector<glm::vec2> Physical::getAxes() {
    auto &[epoch, axes] = m_Cache["Axes"];
    if (epoch == Core::Context::Counter) {
        return axes;
    }
    const auto corners = getCorners();
    for (int i = 0; i < 4; i++) {
        glm::vec2 edge = glm::normalize(corners[(i + 1) % 4] - corners[i]);
        axes[i] = {-edge.y, edge.x};
    }
    epoch = Core::Context::Counter;
    return axes;
}
void Physical::SetPosition(glm::vec2 position) {
    m_Position = position;
}
void Physical::SetVelocity(glm::vec2 velocity) {
    m_Velocity = velocity;
}
std::pair<float_t, float_t>
Physical::project(const std::vector<glm::vec2> &corners,
                  const glm::vec2 &axis) const {
    float minProj = glm::dot(corners[0], axis);
    float maxProj = minProj;
    for (const auto &corner : corners) {
        float proj = glm::dot(corner, axis);
        minProj = std::min(minProj, proj);
        maxProj = std::max(maxProj, proj);
    }
    return {minProj, maxProj};
}
bool Physical::overlapOnAxis(const std::shared_ptr<Physical> &other,
                             const glm::vec2 &axis) {
    auto cornersSelf = getCorners();
    auto cornersOther = other->getCorners();
    auto [minProjSelf, maxProjSelf] = project(cornersSelf, axis);
    auto [minProjOther, maxProjOther] = project(cornersOther, axis);
    return maxProjSelf >= minProjOther && maxProjOther >= minProjSelf;
}
} // namespace Game::Util
