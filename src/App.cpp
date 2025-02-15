#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <memory>

void App::Start() {
    LOG_TRACE("Start");
    Game::CAT.Start();
    m_CurrentState = State::CYCLE;
}

void App::Update() {
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        Game::CAT.EnemyGen();
        LOG_DEBUG("Left button pressed");
    }
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_RB)) {
        LOG_DEBUG("Right button pressed");
        Game::CAT.HurtEnemy();
    }
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_MB)) {
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
    Game::CAT.Update();
}

void App::Draw() {
    Game::CAT.Draw();
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}
