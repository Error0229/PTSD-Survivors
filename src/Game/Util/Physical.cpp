#include "Game/Util/Physical.hpp"

namespace Game::Util {

Physical::Physical() {
    m_Position = {0, 0};
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

bool Physical::IsCollideWith(const std::shared_ptr<Physical> &other) {
    // return IsOverlapped(m_Position.x, m_Position.y, Width(), Height(),
    //                     other->GetPosition().x, other->GetPosition().y,
    //                     other->Width(), other->Height());
    auto cornersSelf = getCorners();
    auto cornersOther = other->getCorners();

    // Normals (axes) are the perpendiculars to the edges
    std::vector<glm::vec2> axes = {
        glm::rotate(glm::normalize(cornersSelf[1] - cornersSelf[0]),
                    glm::radians(90.0f)),
        glm::rotate(glm::normalize(cornersSelf[3] - cornersSelf[0]),
                    glm::radians(90.0f)),
        glm::rotate(glm::normalize(cornersOther[1] - cornersOther[0]),
                    glm::radians(90.0f)),
        glm::rotate(glm::normalize(cornersOther[3] - cornersOther[0]),
                    glm::radians(90.0f))};

    for (const auto &axis : axes) {
        if (!overlapOnAxis(other, axis)) {
            return false; // No collision if there is no overlap on one axis
        }
    }

    return true; // Collision if there is overlap on all axes
}
std::vector<glm::vec2> Physical::getCorners() const {
    std::vector<glm::vec2> corners(4);
    glm::vec2 halfExtents(Width() / 2, Height() / 2);
    glm::vec2 right(cos(Rotation()), sin(Rotation()));
    glm::vec2 up = glm::rotate(right, glm::radians(90.0f));

    corners[0] = m_Position - right * halfExtents.x - up * halfExtents.y;
    corners[1] = m_Position + right * halfExtents.x - up * halfExtents.y;
    corners[2] = m_Position + right * halfExtents.x + up * halfExtents.y;
    corners[3] = m_Position - right * halfExtents.x + up * halfExtents.y;

    return corners;
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
        if (proj < minProj)
            minProj = proj;
        if (proj > maxProj)
            maxProj = proj;
    }
    return {minProj, maxProj};
}
bool Physical::overlapOnAxis(const std::shared_ptr<Physical> &other,
                             const glm::vec2 &axis) const {
    auto cornersSelf = getCorners();
    auto cornersOther = other->getCorners();
    auto [minProjSelf, maxProjSelf] = project(cornersSelf, axis);
    auto [minProjOther, maxProjOther] = project(cornersOther, axis);
    return maxProjSelf >= minProjOther && maxProjOther >= minProjSelf;
}
} // namespace Game::Util
