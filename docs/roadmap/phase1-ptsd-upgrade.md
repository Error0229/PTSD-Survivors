# Phase 1: PTSD v0.3-rc2 Upgrade

**Goal**: Migrate from pre-v0.1 PTSD to v0.3-rc2 while keeping desktop builds working.

---

## Stage 1.1: Dependency Versions

**File**: `cmake/Dependencies.cmake`

Update FetchContent declarations:

| Dependency | Current | Target | Notes |
|---|---|---|---|
| SDL2 | 2.26.5 | 2.32.4 | URL + hash update |
| SDL2_image | 2.6.3 | 2.6.3 | No change |
| SDL2_ttf | 2.20.2 | pinned commit (match PTSD upstream) | Check PTSD's Dependencies.cmake for exact commit |
| SDL2_mixer | 2.6.3 | 2.6.3 | No change, but enable OGG/Vorbis for browser audio |
| GLM | 0.9.9.8 | 1.0.1 | Breaking: namespace changes, deprecated functions |
| spdlog | 1.10.0 | 1.10.0 | No change |
| GoogleTest | 1.13.0 | 1.13.0 | No change |
| ImGui | 1.90.4 | 1.90.4-docking | Branch change |
| nlohmann/json | N/A | 3.11.3 | **New dependency** |

**Action items**:
1. Copy exact FetchContent URLs/tags from PTSD v0.3-rc2's `cmake/Dependencies.cmake`
2. Add `nlohmann/json` FetchContent declaration
3. Remove `tbb` (Intel TBB) from link libraries — parallel execution was already removed
4. Build and fix any immediate compilation errors from version bumps

**GLM 1.0.1 breaking changes to watch for**:
- `glm::vec2` constructors may behave differently
- Deprecated implicit conversions — grep for any `glm::` usage patterns that changed

---

## Stage 1.2: Root → Renderer Rename

**Current**: `Util::Root` class in `include/Util/Root.hpp` / `src/Util/Root.cpp`
**Target**: `Util::Renderer` class

**Files to modify**:

| File | Change |
|---|---|
| `include/Util/Root.hpp` | Rename file → `Renderer.hpp`, rename class `Root` → `Renderer` |
| `src/Util/Root.cpp` | Rename file → `Renderer.cpp`, update class name |
| `include/App.hpp` | `#include "Util/Root.hpp"` → `#include "Util/Renderer.hpp"`, member type |
| `src/App.cpp` | All `Root` references → `Renderer` |
| `CMakeLists.txt` | Update source file lists |

**API change**: In PTSD v0.3-rc2, `Renderer::Update()` is the draw method (not `Draw()`). It:
- Traverses all children + nested children via stack
- Sorts by z-index into a priority queue
- Calls `GameObject::Draw()` on each

**Current code** likely calls something like `m_Root.Draw()` — change to `m_Renderer.Update()`.

---

## Stage 1.3: Drawable::Draw() Signature

**Current**: `Draw(const Util::Transform& transform, float zIndex)` (or similar)
**Target**: `Draw(const Core::Matrices& data)` where:
```cpp
struct Core::Matrices {
    glm::mat4 m_Model;
    glm::mat4 m_Projection;
};
```

**Impact**: Every class that overrides `Drawable::Draw()` must change.

**Files to update**:

| File | Class | Notes |
|---|---|---|
| `include/Core/Drawable.hpp` | `Drawable` | Change virtual signature |
| `include/Util/Image.hpp` / `src/Util/Image.cpp` | `Image` | Override signature |
| `include/Util/Text.hpp` / `src/Util/Text.cpp` | `Text` | Override signature |
| `include/Game/Util/Animation.hpp` / `src/Game/Util/Animation.cpp` | `Animation` | Override signature |

**Important**: With PTSD v0.3-rc2, `GameObject::Draw()` handles the transform→matrices conversion internally via `ConvertToUniformBufferData()`. Game code should no longer compute matrices manually.

---

## Stage 1.4: GameObject Pivot Parameter

**Current constructor**: `GameObject()` or `GameObject(drawable, zIndex)`
**Target constructor**:
```cpp
GameObject(const std::shared_ptr<Core::Drawable>& drawable,
           float zIndex,
           const glm::vec2& pivot = {0, 0},
           bool visible = true,
           const std::vector<std::shared_ptr<GameObject>>& children = {});
```

**Impact**: Every `GameObject` subclass constructor must pass pivot.

**Files to update**:

| File | Class |
|---|---|
| `include/Game/Character.hpp` / `src/Game/Character.cpp` | `Character` |
| `include/Game/Enemy/Enemy.hpp` / `src/Game/Enemy/Enemy.cpp` | `Enemy` |
| `include/Game/Weapon/Weapon.hpp` / `src/Game/Weapon/Weapon.cpp` | `Weapon` |
| `include/Game/Weapon/Whip.hpp` | `Whip` (weapon) |
| `include/Game/Weapon/Vampirica.hpp` | `Vampirica` (weapon) |
| `include/Game/Projectile/Projectile.hpp` / `src/Game/Projectile/Projectile.cpp` | `Projectile` |
| `include/Game/Projectile/Whip.hpp` | `Whip` (projectile) |
| `include/Game/Projectile/Vampirica.hpp` | `Vampirica` (projectile) |
| `include/Game/Passive/Passive.hpp` / `src/Game/Passive/Passive.cpp` | `Passive` |
| `include/Game/Map.hpp` / `src/Game/Map.cpp` | `Map` |

**Default `{0, 0}` centers the sprite** — for most entities this is correct. Weapons/projectiles with offset spawn points may need non-zero pivots.

**Pivot is in pixel coordinates**, applied as: `translate(model, -pivot / drawable_size)`.

---

## Stage 1.5: PTSDPosition / SDLPosition Typed Coordinates

**New types** from `include/Util/Position.hpp`:

- `PTSDPosition(float x, float y)` — origin at window center, Y-up (game world coordinates)
- `SDLPosition` — origin at top-left, Y-down (screen coordinates, construct via `PTSDPosition::ToSDLPosition()`)

**Current code** uses raw `glm::vec2` everywhere. The implicit `PTSDPosition ↔ glm::vec2` conversion is **deprecated** but still works.

**Migration strategy**: Gradual adoption.
1. Don't convert everything at once — the deprecated implicit conversions give us time
2. Start with **new code** using `PTSDPosition` explicitly
3. Convert high-traffic paths: `Camera`, `Character::GetPosition()`, `Enemy::SetPosition()`
4. Convert `Input::GetCursorPosition()` call sites — this now returns `PTSDPosition`

**Files with heaviest coordinate usage**:
- `src/Game/Camera.cpp` — `ScreenToWorld()`, `WorldToScreen()`
- `src/Game/Character.cpp` — movement, position
- `src/Game/Enemy/Enemy.cpp` — spawn position, movement
- `src/Game/Manager.cpp` — collision, spawn logic

---

## Stage 1.6: Animation / Image useAA Parameter

**New**: `Image(filepath, bool useAA = true)` and Animation frame images inherit this.

**For pixel-art Vampire Survivors clone**: We want `useAA = false` (GL_NEAREST filtering).

**Action**: Set `useAA = false` for all sprite loading. Grep for all `Image(` and `Animation(` constructors and add the parameter.

**Key locations**:
- `src/Game/Util/Animation.cpp` — where frame `Image` objects are created
- `src/Util/Image.cpp` — any direct Image construction
- `src/Game/Resource.cpp` — factory creates animations/images

---

## Stage 1.7: config.json

**New file**: `config.json` in project root (or alongside executable)

```json
{
  "title": "PTSD Survivors",
  "window_width": 1280,
  "window_height": 720,
  "fps_cap": 60,
  "default_log_level": 2
}
```

Populates `PTSD_Config::TITLE`, `WINDOW_WIDTH`, `WINDOW_HEIGHT`, `FPS_CAP`, `DEFAULT_LOG_LEVEL`.

**Impact on existing code**:
- Remove hardcoded window dimensions from `Context` initialization
- `Camera` calculations referencing window size should use `PTSD_Config::WINDOW_WIDTH/HEIGHT`

---

## Stage 1.8: Build Verification

1. Build on desktop (Windows, MSVC)
2. Run the game — verify character moves, map renders, enemies spawn
3. Verify animations play correctly (useAA=false should make sprites crisp)
4. Verify FPS counter works with new `Time` API
5. Check for deprecated warnings from `glm::vec2` ↔ `PTSDPosition` conversions

**Known issues to fix during build**:
- `m_Transform.scale.x` access patterns may differ if `Transform` struct changed
- `GameObject::m_Transform` is now a **public member** in v0.3-rc2 (was it before?)
- Uniform buffer binding points may have changed

---

## Estimated Scope

| Stage | Files Touched | Risk |
|---|---|---|
| 1.1 Dependencies | 1-2 | Low (version bumps) |
| 1.2 Root→Renderer | 4-5 | Low (mechanical rename) |
| 1.3 Draw() signature | 4-6 | Medium (API contract change) |
| 1.4 Pivot parameter | 10-12 | Low (default {0,0} preserves behavior) |
| 1.5 Typed coordinates | 4-8 | Low (deprecated conversions still work) |
| 1.6 useAA | 3-5 | Low (add parameter) |
| 1.7 config.json | 1 new + 2-3 | Low |
| 1.8 Build verification | 0 | N/A |
