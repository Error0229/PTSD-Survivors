#include "Util/Animation.hpp"

#include "Util/Time.hpp"

namespace Util {
Animation::Animation(const std::vector<std::string> &paths, bool play,
                     std::size_t interval, bool looping, std::size_t cooldown)
    : m_StartTime(Util::Time::GetElapsedTimeMs()),
      m_Interval(interval),
      m_Looping(looping),
      m_Cooldown(cooldown),
      m_Index(0) {
    m_Frames.reserve(paths.size());
    for (const auto &path : paths) {
        m_Frames.push_back(std::make_shared<Util::Image>(path));
    }
    m_State = (play) ? State::PLAY : State::IDLE;

    Update();
}

void Animation::Draw(const Util::Transform &transform, const float zIndex) {
    m_Frames[m_Index]->Draw(transform, zIndex);
    Update();
}

void Animation::Update() {
    auto now = Util::Time::GetElapsedTimeMs();
    auto delta = now - m_LastFrameTime;
    switch (m_State) {
    case State::CD:
        if (m_LastFrameTime + m_Interval + m_Cooldown < now) {
            m_Index = 0;
            m_State = State::PLAY;
            [[fallthrough]];
        } else {
            break;
        }
    case State::PLAY:
        if (m_PauseOffset > 0) {
            delta -= m_PauseOffset;
            m_PauseOffset = 0;
        }
        if (delta > m_Interval) {
            m_Index++;
            if (m_Index == m_Frames.size()) {
                m_Index -= 1;
                m_State = (m_Looping) ? State::CD : State::END;
                break;
            }
            m_LastFrameTime = now;
        }
        break;
    case State::END:
    case State::PAUSE:
    case State::IDLE:
        break;
    }
}
void Animation::Play() {
    auto now = Util::Time::GetElapsedTimeMs();
    switch (m_State) {
    case State::PLAY:
    case State::CD:
        break;
    case State::PAUSE:
        m_PauseOffset = now - m_PauseTime;
        m_State = State::PLAY;
        break;
    case State::END:
    case State::IDLE:
        m_Index = 0;
        m_State = State::PLAY;
        break;
    }
}
void Animation::Pause() {
    switch (m_State) {
    case State::PLAY:
    case State::CD:
        m_State = State::PAUSE;
        m_PauseTime = Util::Time::GetElapsedTimeMs();
        break;
    case State::PAUSE:
    case State::END:
    case State::IDLE:
        break;
    }
}
} // namespace Util
