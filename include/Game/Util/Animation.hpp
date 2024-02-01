#ifndef ANIMATION_HPP
#define ANIMATION_HPP
#include "Core/Drawable.hpp"
#include "Util/Transform.hpp"
#include "pch.hpp"
#include <memory>
namespace Game::Util {
class Animation {
public:
    Animation() = default;
    Animation(std::vector<std::shared_ptr<::Core::Drawable>> frames,
              bool isLoop = true, time_t frameTime = 100);
    void Update();
    void Stop();
    void Pause();
    void Play();
    void SetFrame(int32_t frame);
    int32_t GetFrame();
    int32_t FrameCount();
    void Draw(::Util::Transform &transform, float_t &zIndex);

private:
    std::vector<std::shared_ptr<::Core::Drawable>> m_Frames;
    bool m_IsAnimated = false, m_IsLoop;
    int32_t m_CurrentFrame = 0;
    time_t m_FrameTime, m_LastFrameTime;
};
} // namespace Game::Util
#endif
