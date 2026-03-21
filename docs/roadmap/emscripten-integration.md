# Emscripten / WebAssembly Integration Plan

## Overview

Compile the game to WebAssembly via Emscripten, targeting WebGL 2.0 (OpenGL ES 3.0).

## CMake Changes

```cmake
if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  target_link_options(${PROJECT_NAME} PRIVATE
    -sUSE_SDL=2
    -sUSE_SDL_IMAGE=2
    -sSDL2_IMAGE_FORMATS=["png"]
    -sUSE_SDL_TTF=2
    -sUSE_SDL_MIXER=2
    -sALLOW_MEMORY_GROWTH=1
    -sMIN_WEBGL_VERSION=2
    -sMAX_WEBGL_VERSION=2
    --preload-file ${CMAKE_SOURCE_DIR}/resources@/resources
  )
  # Emscripten provides SDL2 and GL — skip FetchContent for those
  # Skip GLEW entirely (Emscripten provides GL headers natively)
endif()
```

Build command:
```bash
emcmake cmake -B build-web
cmake --build build-web
```

## Shader Downgrade: GLSL 410 → GLSL ES 300

Current shaders use `#version 410 core`. WebGL 2.0 requires GLSL ES 300.

Key changes:
- `#version 300 es` instead of `#version 410 core`
- Add `precision mediump float;` in fragment shaders
- `texture2D()` → `texture()` (already correct in 410)
- Remove any GL 4.x-specific features

Use `#ifdef GL_ES` to support both desktop and web from the same shader files.

## Main Loop

```cpp
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// In main():
#ifdef __EMSCRIPTEN__
emscripten_set_main_loop_arg(
    [](void* arg) { static_cast<App*>(arg)->MainLoopIteration(); },
    &app, 0, true);  // 0 = requestAnimationFrame (vsync)
#else
while (!context->GetExit()) {
    app.MainLoopIteration();
    context->Update();
}
#endif
```

## GL Context

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

## GLEW

```cpp
#ifndef __EMSCRIPTEN__
#include <GL/glew.h>  // Desktop only
#endif
#include <SDL_opengl.h>
```

Skip GLEW initialization entirely on Emscripten — the GL functions are available directly.

## Asset Pipeline

- All sprites: **PNG** (not BMP). Use sprite sheets / texture atlases.
- Audio: **MP3** or **OGG** (not WAV)
- `--preload-file` bundles everything into a single `.data` file
- Target total download: **< 20 MB**

## Performance Notes

- WebGL has high per-draw-call overhead vs native GL
- Sprite batching is critical — batch all sprites sharing a texture into one `glDrawArraysInstanced` call
- `requestAnimationFrame` caps at display refresh rate (usually 60 Hz)
- Avoid `-sASYNCIFY` (causes "unreachable" errors in some configs)
- Use `-O2` optimization level for release builds
