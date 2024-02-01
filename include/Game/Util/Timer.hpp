#ifndef TIMER_HPP
#define TIMER_HPP
#include "pch.hpp"
#include <memory>

namespace Game::Util {
class Timer {
public:
    Timer();
    void Start();
    void Reset();
    void Pause();
    void Resume();
    bool IsRunning();
    time_t Now();
    int32_t GetSeconds();
    void Transition(time_t Diff);
    std::string GetMinuteString();
    std::string GetSecondString();

private:
    time_t m_StartTicks;
    time_t m_PausedTicks;
    bool m_IsRunning;
};
} // namespace Game::Util
#endif
