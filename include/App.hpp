#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include "Game/Camera.hpp"
#include "Game/Character.hpp"
#include "Game/Map.hpp"
#include <memory>

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void Update();
    void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
    State m_CurrentState = State::START;
    std::shared_ptr<Game::Character> m_Player;
    std::unique_ptr<Game::Map> m_Map;
};

#endif
