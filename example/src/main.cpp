#include "App.hpp"

#include "Core/Context.hpp"

#include "Util/Input.hpp"

int main(int, char **) {
    auto context = Core::Context::GetInstance();
    App app;

    // set icon in window.
    context->SetWindowIcon(ASSETS_DIR "/icon.jpg");

    while (!context->GetExit()) {
        context->Setup();

        switch (app.GetCurrentState()) {
        case App::State::START:
            app.Start();
            break;

        case App::State::CYCLE:
            app.Update();
            app.Draw();
            break;

        case App::State::END:
            app.End();
            context->SetExit(true);
            break;
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        context->Update();
    }
    return 0;
}
