# Phase 2: Emscripten Foundation

**Goal**: Get the upgraded PTSD codebase rendering in a browser via WebAssembly.

**Prerequisite**: Phase 1 complete (PTSD v0.3-rc2 upgrade).

---

## Stage 2.1: Shader Downgrade (GLSL 410 → GLSL ES 300)

**Current shaders** (in PTSD's `assets/shaders/`):
- `Base.vert` — `#version 410 core`
- `Base.frag` — `#version 410 core`
- `Triangle.vert` / `Triangle.frag` — `#version 410 core`

**Target**: Dual-compat shaders that work on both desktop GL 4.1 and WebGL 2.0.

**Strategy**: Preprocessor-based version switching in each shader:

```glsl
#ifdef GL_ES
#version 300 es
precision mediump float;
#else
#version 410 core
#endif
```

**Specific changes per shader**:

| Shader | Changes Needed |
|---|---|
| `Base.vert` | Version directive, remove `core` qualifier |
| `Base.frag` | Version directive, add `precision mediump float;` |
| `Triangle.vert` | Version directive |
| `Triangle.frag` | Version directive, add precision |

**Note**: `texture()` is already correct in GLSL 410 (no `texture2D()` to fix). UBO syntax (`layout(std140) uniform`) is supported in both GLSL 410 and ES 300.

**Risk**: PTSD owns these shaders in its `assets/` directory. Options:
1. Fork PTSD and modify shaders there
2. Override shader paths in our build to point to our own copies
3. Contribute the dual-compat change upstream to PTSD

**Recommendation**: Option 3 (upstream PR) is ideal. Option 2 as fallback — copy shaders to `resources/Shaders/` and override the load path.

---

## Stage 2.2: GL Context Ifdefs

**File**: PTSD's `src/Core/Context.cpp` (need to modify or override)

**Current**:
```cpp
SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
```

**Target**:
```cpp
#ifdef __EMSCRIPTEN__
SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif
```

**GLEW skip**: Emscripten provides GL functions natively.
```cpp
#ifndef __EMSCRIPTEN__
#include <GL/glew.h>
glewInit();  // skip on Emscripten
#endif
```

**Same challenge as 2.1** — these are PTSD framework files, not our game files. Same three options (fork / override / upstream PR).

---

## Stage 2.3: CMake Emscripten Target

**File**: `CMakeLists.txt`

Add Emscripten-specific block:

```cmake
if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")

  # Emscripten provides SDL2 and GL — skip FetchContent for those
  # Skip GLEW entirely

  target_link_options(${PROJECT_NAME} PRIVATE
    -sUSE_SDL=2
    -sUSE_SDL_IMAGE=2
    -sSDL2_IMAGE_FORMATS=["png"]
    -sUSE_SDL_TTF=2
    -sUSE_SDL_MIXER=2
    -sMIN_WEBGL_VERSION=2
    -sMAX_WEBGL_VERSION=2
    -sALLOW_MEMORY_GROWTH=1
    --preload-file ${CMAKE_SOURCE_DIR}/resources@/resources
  )

  # Release optimization
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE -O2)
  endif()
else()
  # Existing desktop dependencies (GLEW, SDL2 via FetchContent, etc.)
endif()
```

**Dependency skip logic**: When `EMSCRIPTEN` is defined, Emscripten's ports system provides SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, and GL headers. We must:
1. Guard `FetchContent` for SDL2/GLEW with `if(NOT EMSCRIPTEN)`
2. Keep FetchContent for: GLM, spdlog, nlohmann/json, GoogleTest, ImGui (Emscripten doesn't provide these)
3. Add ImGui OpenGL ES backend selection

**Build commands**:
```bash
emcmake cmake -B build-web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web
```

**Output**: `build-web/Sample.html`, `Sample.js`, `Sample.wasm`, `Sample.data`

---

## Stage 2.4: Main Loop Conversion

**File**: `src/main.cpp`

**Current**:
```cpp
while (!context->GetExit()) {
    switch (app.GetCurrentState()) {
    case App::State::START: app.Start(); break;
    case App::State::CYCLE: app.Update(); app.Draw(); break;
    case App::State::END:   app.End(); context->SetExit(true); break;
    }
    context->Update();
}
```

**Target**:
```cpp
#ifdef __EMSCRIPTEN__
#include <emscripten.h>

struct MainLoopData {
    App* app;
    Core::Context* context;
};

void MainLoopIteration(void* arg) {
    auto* data = static_cast<MainLoopData*>(arg);
    auto& app = *data->app;
    auto* context = data->context;

    switch (app.GetCurrentState()) {
    case App::State::START: app.Start(); break;
    case App::State::CYCLE: app.Update(); app.Draw(); break;
    case App::State::END:   app.End(); emscripten_cancel_main_loop(); return;
    }
    context->Update();
}
#endif

int main() {
    auto context = Core::Context::GetInstance();
    App app;

#ifdef __EMSCRIPTEN__
    MainLoopData loopData{&app, context.get()};
    emscripten_set_main_loop_arg(MainLoopIteration, &loopData, 0, true);
    // 0 = use requestAnimationFrame (vsync to display refresh)
    // true = simulate infinite loop
#else
    while (!context->GetExit()) {
        // ... existing loop ...
    }
#endif
}
```

**Key**: `emscripten_set_main_loop_arg` with fps=0 uses `requestAnimationFrame`, which syncs to display refresh (usually 60 Hz). Do **not** use `-sASYNCIFY` — it causes "unreachable" errors.

---

## Stage 2.5: Asset Conversion

### BMP → PNG

Grep for any remaining `.bmp` references. The original OOPL-VS used 3,000+ BMP files; our project should already be PNG but verify.

```bash
find resources/ -name "*.bmp" -type f
grep -r "\.bmp" src/ include/
```

Convert any remaining BMPs:
```bash
for f in resources/**/*.bmp; do
  convert "$f" "${f%.bmp}.png"
  rm "$f"
done
```

Update any hardcoded `.bmp` paths in source.

### Audio format

SDL2_mixer on Emscripten supports MP3 and WAV. Current audio is WAV (large files).

**Decision**: Keep WAV for now (simpler), optimize to MP3/OGG in Phase 6 (Content & Balance) when targeting < 20 MB download.

### Font path

**Known bug**: Hardcoded `"../resources/Font/ANY.ttf"` in Manager.cpp.

Fix to use relative path that works from both desktop build dir and Emscripten preloaded filesystem:
```cpp
// Emscripten preloads to /resources/...
// Desktop runs from build/ so ../resources/... works
#ifdef __EMSCRIPTEN__
const std::string FONT_PATH = "/resources/Font/ANY.ttf";
#else
const std::string FONT_PATH = "../resources/Font/ANY.ttf";
#endif
```

Or better: use `config.json` to define resource base path.

---

## Stage 2.6: ImGui on Emscripten

PTSD v0.3-rc2 uses ImGui for debug UI. ImGui has Emscripten support but needs:

1. Use `imgui_impl_opengl3.h` with `#version 300 es` shader version
2. Use `imgui_impl_sdl2.h` (same as desktop)
3. Set `ImGui_ImplOpenGL3_Init("#version 300 es")` on Emscripten
4. On desktop keep `ImGui_ImplOpenGL3_Init("#version 410")`

**If ImGui is too heavy for browser**: Add a `PTSD_DISABLE_IMGUI` CMake option to strip it entirely from the WASM build. ImGui is primarily for dev — the game UI will be custom.

---

## Stage 2.7: Browser Verification

**Test matrix**:

| Test | Expected |
|---|---|
| Build completes without errors | `Sample.html` + `.wasm` + `.data` generated |
| Open in Chrome | WebGL 2.0 context created |
| Window renders | Black screen or background visible |
| Sprites render | Character, map, enemies visible |
| Input works | WASD movement, mouse cursor tracked |
| Audio plays | At least WAV playback works |
| No console errors | No GL errors, no `unreachable` traps |

**Local test server** (Emscripten output needs HTTP, not file://):
```bash
cd build-web
python -m http.server 8080
# Open http://localhost:8080/Sample.html
```

---

## PTSD Framework Modification Strategy

Stages 2.1 and 2.2 require modifying PTSD framework files. Options:

| Option | Pros | Cons |
|---|---|---|
| **A: Fork PTSD** | Full control, can iterate fast | Maintenance burden, drift from upstream |
| **B: Override files** | No fork needed, surgical | Fragile, must track upstream shader changes |
| **C: Upstream PR** | Benefits everyone, no fork | Slower, may be rejected, need maintainer buy-in |

**Recommended**: Start with **A** (fork to `error0229/practical-tools-for-simple-design`), make browser-compat changes, then submit as upstream PR (**C**). If accepted, switch back to upstream. If rejected, maintain fork.

The fork needs these changes:
1. Shader version switching (`#ifdef GL_ES`)
2. GL context version ifdef (`__EMSCRIPTEN__`)
3. GLEW skip on Emscripten
4. ImGui GL ES init
