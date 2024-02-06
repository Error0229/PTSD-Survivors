#ifndef ANIMATED_HPP
#define ANIMATED_HPP
#include "Game/Config.hpp"
#include "Game/Util/Animation.hpp"
#include "Util/Transform.hpp"
#include "pch.hpp"

#include <cmath>
#include <memory>
namespace Game::Util {
class Animated {
public:
    void Load(const std::string &name, const std::vector<std::string> &frames,
              bool isLoop = true, time_t frameTime = 100);
    void Load(const std::string &name, std::shared_ptr<Animation> animation);
    void SetAnimation(const std::string &name);
    void Draw(::Util::Transform &transform, float_t &zIndex,
              const std::string &name = NULL_STRING);
    void Play(const std::string &name = NULL_STRING);
    void Stop(const std::string &name = NULL_STRING);
    void Pause(const std::string &name = NULL_STRING);
    void SetFrame(int32_t frame, const std::string &name = NULL_STRING);
    int32_t GetFrame(const std::string &name = NULL_STRING);
    int32_t FrameCount(const std::string &name = NULL_STRING);
    void Update(const std::string &name = NULL_STRING);
    bool IsAnimated(const std::string &name = NULL_STRING);
    float_t Height(const std::string &name = NULL_STRING);
    float_t Width(const std::string &name = NULL_STRING);

protected:
    std::shared_ptr<Animation> GetAnimation(const std::string &name);
    std::map<std::string, std::shared_ptr<Animation>> m_Animation;
    std::string m_CurrentAnimation = NULL_STRING;
};
} // namespace Game::Util

#endif
