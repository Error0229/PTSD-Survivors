#include "Game/Util/Timer.hpp"
#include "SDL_timer.h"

namespace Game::Util {
Timer Clock;
Timer::Timer() {
    m_StartTicks = 0;
    m_PausedTicks = 0;
    m_IsRunning = false;
}
void Timer::Start() {
    if (!m_IsRunning) {
        m_IsRunning = true;
        m_StartTicks = SDL_GetTicks();
    }
}
void Timer::Reset() {
    m_IsRunning = false;
    m_StartTicks = 0;
    m_PausedTicks = 0;
}
void Timer::Pause() {
    if (m_IsRunning) {
        m_IsRunning = false;
        m_PausedTicks = SDL_GetTicks() - m_StartTicks;
    }
}
void Timer::Resume() {
    if (!m_IsRunning) {
        m_IsRunning = true;
        m_StartTicks = SDL_GetTicks() - m_PausedTicks;
        m_PausedTicks = 0;
    }
}
bool Timer::IsRunning() {
    return m_IsRunning;
}
time_t Timer::Now() {
    if (m_IsRunning)
        return SDL_GetTicks() - m_StartTicks;
    else
        return m_PausedTicks;
}
int32_t Timer::GetSeconds() {
    return Now() / 1000;
}
std::string Timer::GetMinuteString() {
    std::string s = std::to_string(Now() / 1000 / 60);
    if (s.size() == 1)
        s = "0" + s;
    return s;
}
std::string Timer::GetSecondString() {
    std::string s = std::to_string(Now() / 1000 % 60);
    if (s.size() == 1)
        s = "0" + s;
    return s;
}
void Timer::Transition(time_t Diff) {
    m_StartTicks -= Diff;
}
} // namespace Game::Util
