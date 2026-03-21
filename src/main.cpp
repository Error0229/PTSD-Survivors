#include "App.hpp"

#include "Core/Context.hpp"
#include "Util/Logger.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <exception>

struct MainLoopData {
    App *app;
    std::shared_ptr<Core::Context> context;
};

void MainLoopIteration(void *arg) {
    auto *data = static_cast<MainLoopData *>(arg);
    auto &app = *data->app;
    auto &context = *data->context;

    try {
        switch (app.GetCurrentState()) {
        case App::State::START:
            app.Start();
            break;

        case App::State::CYCLE:
            context.Setup();
            app.Update();
            app.Draw();
            break;

        case App::State::END:
            app.End();
#ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
#else
            context.SetExit(true);
#endif
            break;
        }
        context.Update();
    } catch (const std::exception &e) {
        LOG_ERROR("Exception in main loop: {}", e.what());
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#endif
    } catch (...) {
        LOG_ERROR("Unknown exception in main loop");
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#endif
    }
}

int main(int, char **) {
    auto context = Core::Context::GetInstance();
    App app;

    MainLoopData loopData{&app, context};

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(MainLoopIteration, &loopData, 0, true);
#else
    while (!context->GetExit()) {
        MainLoopIteration(&loopData);
    }
#endif
    return 0;
}
