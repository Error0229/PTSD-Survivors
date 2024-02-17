#include "Game/Util/Animation.hpp"
#include "Game/Util/Timer.hpp"
#include "Util/Image.hpp"
namespace Game::Util {
Animation::Animation(std::vector<std::shared_ptr<::Util::Image>> frames,
                     bool isLoop, time_t frameTime)
    : m_Frames(frames), m_IsAnimated(false), m_IsLoop(isLoop),
      m_FrameTime(frameTime), m_LastFrameTime(0), m_PauseTime(0) {}
Animation::Animation(std::vector<std::string> &paths, bool isLoop,
                     time_t frameTime)
    : m_IsAnimated(false), m_IsLoop(isLoop), m_FrameTime(frameTime),
      m_LastFrameTime(0), m_PauseTime(0) {
    for (auto &path : paths) {
        m_Frames.push_back(std::make_shared<::Util::Image>(path));
    }
}
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
    m_PauseTime = Clock.Now();
    m_IsAnimated = false;
}
void Animation::Play() {
    if (!m_IsAnimated) {
        m_LastFrameTime += Clock.Now() - m_PauseTime;
    }
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
float_t Animation::GetHeight() {
    if (m_CurrentFrame < 0 || m_CurrentFrame >= FrameCount())
        return 0;
    return m_Frames[m_CurrentFrame]->GetSize().y;
}
float_t Animation::GetWidth() {
    if (m_CurrentFrame < 0 || m_CurrentFrame >= FrameCount())
        return 0;
    return m_Frames[m_CurrentFrame]->GetSize().x;
}
bool Animation::IsAnimated() {
    return m_IsAnimated;
}
void Animation::Draw(::Util::Transform &transform, const float_t &zIndex) {
    Update();
    if (m_CurrentFrame < 0 || m_CurrentFrame >= FrameCount())
        return;
    m_Frames[m_CurrentFrame]->Draw(transform, zIndex);
}
void Animation::SetFrameTime(time_t frameTime) {
    m_FrameTime = frameTime;
}
} // namespace Game::Util
