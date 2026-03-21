# Implementation Roadmap

Linear development — infrastructure first, then game systems. Each phase builds on the previous one. All new game code is written against the final PTSD v0.3 APIs from the start.

## Phase 1: PTSD Upgrade

**Goal**: Migrate codebase to PTSD v0.3-rc2 while keeping desktop builds working.

- [ ] Update `cmake/Dependencies.cmake` with new versions (SDL2 2.32.4, GLM 1.0.1, etc.)
- [ ] Rename `Root` → `Renderer` throughout codebase
- [ ] Update `Drawable::Draw()` signatures (`const Core::Matrices& data`)
- [ ] Add `pivot` parameter to `GameObject` constructors
- [ ] Adopt `PTSDPosition` / `SDLPosition` typed coordinates
- [ ] Add `config.json` for runtime settings
- [ ] Add `nlohmann/json` dependency
- [ ] Fix remaining compilation errors, verify desktop build

## Phase 2: Emscripten Foundation

**Goal**: Get the game rendering in a browser.

- [ ] Downgrade shaders from GLSL 410 → GLSL ES 300 (WebGL 2.0 compat)
- [ ] Add `#ifdef __EMSCRIPTEN__` guards for GL context creation (skip GLEW, use ES profile)
- [ ] Add Emscripten build target to CMakeLists.txt (`--use-port=sdl2`, `--preload-file`, `-sMIN_WEBGL_VERSION=2`)
- [ ] Implement `emscripten_set_main_loop` in main.cpp
- [ ] Convert remaining BMP assets to PNG
- [ ] Verify basic rendering in browser

## Phase 3: Rendering Performance

**Goal**: Achieve 60 FPS with hundreds of sprites in WebGL.

- [ ] Implement sprite batching (batch sprites sharing a texture into one draw call)
- [ ] Build texture atlas system with source-rect rendering
- [ ] Z-ordered render queue that batches by texture
- [ ] Profile and optimize draw call count (WebGL has high per-call overhead)

## Phase 4: Complete Game Systems

**Goal**: Feature parity with the original OOPL-VS game.

- [ ] Wave system — timed enemy spawning with escalating difficulty (data-driven JSON)
- [ ] Full weapon roster — port all 11 weapons (whip, magic wand, knife, axe, garlic, holy water, holy book, cross, fire wand, king bible, heaven sword)
- [ ] Projectile behaviors — linear, orbital, area, boomerang, radial patterns (Strategy pattern)
- [ ] Passive items — stat modifiers (armor, max HP, cooldown, area, speed, etc.)
- [ ] Pickup system — XP gems, treasure chests, healing items
- [ ] Level-up system — XP → level up → choose 1 of 3/4 random upgrades
- [ ] Evolution system — weapon + passive combo → evolved weapon

## Phase 5: UI & Polish

**Goal**: Complete user-facing experience.

- [ ] Main menu — character selection, map selection
- [ ] HUD — HP bar, XP bar, timer, weapon slots, kill count
- [ ] Level-up UI — card selection with descriptions
- [ ] Pause menu
- [ ] Game over screen with stats summary
- [ ] Chest opening UI

## Phase 6: Content & Balance

**Goal**: Ship-quality gameplay.

- [ ] Port all 11 characters with unique stats
- [ ] Port all 3 maps with proper tiling
- [ ] Port all enemy types with animations
- [ ] Balance wave difficulty curve
- [ ] Balance weapon damage/cooldown scaling
- [ ] Final asset optimization (target < 20 MB total download)

## Deployment

Final WASM build hosted on `error0229.github.io/secret/` as an easter egg. Triggered by clicking the hidden "???" node in the portfolio's D3.js skill network graph.

**Targets**: < 20 MB download, < 3 sec load, 60 FPS gameplay.
