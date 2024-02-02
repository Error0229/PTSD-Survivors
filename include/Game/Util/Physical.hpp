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
    virtual float_t Width() = 0;
    virtual float_t Height() = 0;
    virtual glm::vec2 &GetPosition();

    virtual const std::type_info &Type() final;
    virtual float Distance(std::shared_ptr<Physical> &other);
    virtual bool IsCollideWith(std::shared_ptr<Physical> &other);
    virtual void SetPosition(glm::vec2 position);

protected:
    glm::vec2 m_Position;
};
} // namespace Game::Util

#endif /* PHYSICAL_HPP */
