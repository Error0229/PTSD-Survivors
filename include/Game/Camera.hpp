#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "pch.hpp"

namespace Game {
class Camera  {
public:
    static void Update(const glm::vec2& position);
    static glm::vec2 GetPosition();
    static glm::vec2 ScreenToWorld(glm::vec2 screenPosition);
    static glm::vec2 WorldToScreen(glm::vec2 worldPosition);
    static glm::vec2 Position;
};

} // namespace Game

#endif /* CAMERA_HPP */
