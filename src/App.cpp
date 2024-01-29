#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <memory>

void App::Start() {
    LOG_TRACE("Start");
    m_Map = std::make_unique<Game::Map>();
    m_Player = std::make_shared<Game::Character>();
    m_Map->Start();
    m_Player->Start();
    m_Map->SetMap("map");
    m_Player->Setup("cat");

    m_CurrentState = State::UPDATE;
}

void App::Update() {
    if (Util::Input::IsLButtonPressed()) {
        LOG_DEBUG("Left button pressed");
    }
    if (Util::Input::IsRButtonPressed()) {
        LOG_DEBUG("Right button pressed");
    }
    if (Util::Input::IsMButtonPressed()) {
        LOG_DEBUG("Middle button pressed");
    }
    if (Util::Input::IfScroll()) {
        auto delta = Util::Input::GetScrollDistance();
        LOG_DEBUG("Scrolling: x: {}, y: {}", delta.x, delta.y);
    }
    if (Util::Input::IsMouseMoving()) {
        // LOG_DEBUG("Mouse moving! x:{}, y{}", cursorPos.x, cursorPos.y);
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        LOG_DEBUG("A");
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::B)) {
        LOG_DEBUG("B");
        Util::Input::SetCursorPosition({0.0F, 0.0F});
    }
    m_Player->Update({Util::Input::GetCursorPosition()});
    m_Map->Update({m_Player->GetPosition()});
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}
