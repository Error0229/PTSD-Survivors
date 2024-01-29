#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "pch.hpp"

#include "Util/GameObject.hpp"

namespace Game {
class Character : public Util::GameObject {
public:
    Character() = default;
    ~Character() override = default;
    void Start() override;
    void Update(const Util::Transform &transform = Util::Transform()) override;
    void SetSpeed(float speed);
    void Setup(std::string Name);
    void MoveTowards(glm::vec2 position);

private:
    float m_Speed = 10.0;
    glm::vec2 m_Position;
    std::string m_Name;
};
} // namespace Game

#endif /* CHARACTER_HPP */
