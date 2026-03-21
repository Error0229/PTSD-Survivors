# PTSD Framework Upgrade Plan

## Current State (in this project)

PTSD-Survivors was built ~2 years ago against an early version of PTSD (pre-v0.1 era). The dependencies are significantly outdated.

## Target: PTSD v0.3-rc2 (March 2025)

### Breaking Changes to Handle

**From v0.2 (April 2024):**
- `Root` class → renamed to **`Renderer`**
- `Animation::Reset()` → **removed**
- `Drawable::Draw()` signature changed — now takes `const Core::Matrices& data`
- `GameObject` constructor — new `pivot` parameter

**From v0.3-rc (current main):**
- New `PTSDPosition` / `SDLPosition` typed coordinate system
- `glm::vec2` implicit conversions deprecated (will be removed)
- `Animation` / `Image` constructors now have `useAA` parameter (default `true`)
- Runtime config via `config.json` (window title, size, FPS cap, log level)
- New dependency: `nlohmann/json` 3.11.3

### Dependency Updates

| Dependency | Our Version | Latest |
|---|---|---|
| SDL2 | 2.26.5 | 2.32.4 |
| SDL2_image | 2.6.3 | 2.6.3 (same) |
| SDL2_ttf | 2.20.2 | pinned commit |
| SDL2_mixer | 2.6.3 | 2.6.3 (same) |
| GLM | 0.9.9.8 | 1.0.1 |
| spdlog | 1.10.0 | 1.10.0 (same, 1.11 skipped) |
| GoogleTest | 1.13.0 | 1.13.0 (same) |
| ImGui | 1.90.4 | 1.90.4-docking |
| nlohmann/json | N/A | 3.11.3 (new) |
| C++ standard | C++20 (our choice) | C++17 (PTSD has always been C++17) |

### Migration Steps

1. Update `cmake/Dependencies.cmake` with new versions/URLs
2. Rename `Root` → `Renderer` throughout codebase
3. Update `Drawable::Draw()` signatures
4. Add `pivot` parameter to `GameObject` constructors
5. Adopt `PTSDPosition` / `SDLPosition` typed coordinates
6. Add `config.json` for runtime settings
7. Test build, fix remaining compilation errors

### Note on C++ Standard

PTSD has always been C++17 — it never downgraded. Our project chose C++20 as a local deviation. This is fine: C++17 libraries compile cleanly under a C++20 project. We keep C++20 for our game code (designated initializers, concepts, ranges, etc.) and consume PTSD as-is.
