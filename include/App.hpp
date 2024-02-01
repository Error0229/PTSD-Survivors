#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include "Game/Manager.hpp"
#include "Game/Map.hpp"
#include <memory>

class App {
public:
    enum class State {
        START,
        CYCLE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();

    void Update();
    void Draw();

    void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
    State m_CurrentState = State::START;
    Game::Manager m_Manager;
};

#endif
