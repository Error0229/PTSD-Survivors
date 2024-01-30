#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "Game/Physical.hpp"
#include "pch.hpp"

#include "Util/GameObject.hpp"

namespace Game {
class Character : public Util::GameObject, public Physical {
public:
    Character() = default;
    ~Character() override = default;
    void Start() override;
    void Update(const Util::Transform &transform = Util::Transform()) override;
    void SetSpeed(float speed);
    void Setup(std::string name);
    void GoTo(glm::vec2 target);

    float Width() override;
    float Height() override;

private:
    std::string m_Name;
};
} // namespace Game

#endif /* CHARACTER_HPP */
