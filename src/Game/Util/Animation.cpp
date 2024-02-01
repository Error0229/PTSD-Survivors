#include "Game/Util/Animation.hpp"
#include "Game/Util/Timer.hpp"
#include "Util/Image.hpp"
namespace Game::Util {
static Util::Timer Clock;
Animation::Animation(std::vector<std::shared_ptr<::Util::Image>> frames,
                     bool isLoop, time_t frameTime)
    : m_Frames(frames),
      m_IsAnimated(false),
      m_IsLoop(isLoop),
      m_FrameTime(frameTime) {}
void Animation::Update() {
    auto now = Clock.Now();
    if (!m_IsAnimated || now - m_LastFrameTime <= m_FrameTime)
        return;
    m_LastFrameTime = now;
    m_CurrentFrame++;
    if (m_CurrentFrame >= FrameCount()) {
        if (m_IsLoop) {
            m_CurrentFrame = 0;
        } else {
            m_IsAnimated = false;
        }
    }
}
void Animation::Stop() {
    m_IsAnimated = false;
    m_CurrentFrame = FrameCount() - 1;
}
void Animation::Pause() {
    m_IsAnimated = false;
}
void Animation::Play() {
    m_IsAnimated = true;
}
void Animation::SetFrame(int32_t frame) {
    m_CurrentFrame = frame;
}
int32_t Animation::GetFrame() {
    return m_CurrentFrame;
}
int32_t Animation::FrameCount() {
    return static_cast<int32_t>(m_Frames.size());
}
void Animation::Draw(::Util::Transform &transform, float_t &zIndex) {
    if (m_CurrentFrame < 0 || m_CurrentFrame >= FrameCount())
        return;
    m_Frames[m_CurrentFrame]->Draw(transform, zIndex);
}
} // namespace Game::Util
