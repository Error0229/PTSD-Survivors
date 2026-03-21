# Phase 5: UI & Polish

**Goal**: Complete user-facing experience — menus, HUD, level-up cards, game over.

**Prerequisite**: Phase 4 complete (all game systems functional).

---

## UI Architecture Decision

### Option A: Custom sprite-based UI (recommended)
- Build UI from `Image` + `Text` GameObjects
- Position with PTSD coordinate system
- Handle clicks via `Input::GetCursorPosition()` + bounding box hit tests
- Matches the pixel-art aesthetic
- No external dependency

### Option B: ImGui
- Quick to prototype but wrong aesthetic for a Vampire Survivors clone
- ImGui is primarily a debug/tools UI, not a game UI
- Keep ImGui available for dev tools only (FPS, debug info)

**Decision**: Option A for all player-facing UI. ImGui for dev overlays only (behind a toggle key like F3).

---

## Stage 5.1: Game State Machine

### Current
`App::State` enum: `START`, `CYCLE`, `END`

### Target

```cpp
enum class GameState {
    MAIN_MENU,
    CHARACTER_SELECT,
    MAP_SELECT,
    PLAYING,
    PAUSED,
    LEVEL_UP,          // modal over PLAYING
    CHEST_OPEN,        // modal over PLAYING
    GAME_OVER,
    RESULTS
};
```

**State transitions**:
```
MAIN_MENU → CHARACTER_SELECT → MAP_SELECT → PLAYING
PLAYING → PAUSED → PLAYING
PLAYING → LEVEL_UP → PLAYING
PLAYING → CHEST_OPEN → PLAYING
PLAYING → GAME_OVER → RESULTS → MAIN_MENU
```

### Implementation

Refactor `App.cpp` to use the state machine. Each state has:
- `Enter()` — setup UI elements, register callbacks
- `Update(float dt)` — handle input, update logic
- `Draw()` — render state-specific UI
- `Exit()` — cleanup

**PLAYING** state delegates to `Manager::Update()` / `Manager::Draw()`.

**Modal states** (LEVEL_UP, CHEST_OPEN, PAUSED) render on top of the frozen PLAYING state.

---

## Stage 5.2: HUD

### Elements

```
┌─────────────────────────────────────────────┐
│ [HP BAR]                    ⏱ 05:23   [⏸]  │
│                                              │
│                                              │
│                                              │
│                                              │
│                                              │
│                                              │
│ [🗡][🪄][🔪][🪓][  ][  ]    LV.5  💀 142   │
│ [XP BAR ████████░░░░░░░░░░░░░░░░░░░░░░░░░] │
└─────────────────────────────────────────────┘
```

| Element | Position | Data Source |
|---|---|---|
| HP Bar | Top-left | `Character::GetHP()` / `Character::GetMaxHP()` |
| Timer | Top-center | `Timer::GetMinuteString()` + `:` + `Timer::GetSecondString()` |
| Pause button | Top-right | Click → PAUSED state |
| Weapon slots (6) | Bottom-left | `Manager::m_Weapons` icons |
| Level indicator | Bottom-center | `Character::GetLevel()` |
| Kill count | Bottom-right | `Manager::m_KillCount` |
| XP Bar | Bottom (full width) | `Character::GetXP()` / `Character::GetXPToLevel()` |

### Implementation

**New class**: `Game::UI::HUD`

```cpp
class HUD {
public:
    void Start();
    void Update(const Character& character, const Manager& manager, float gameTime);
    void Draw();

private:
    // HP bar: background (gray) + fill (red/green) + border
    std::shared_ptr<Image> m_HPBarBg, m_HPBarFill, m_HPBarBorder;

    // XP bar: background + fill (blue) + border
    std::shared_ptr<Image> m_XPBarBg, m_XPBarFill, m_XPBarBorder;

    // Text elements
    std::shared_ptr<Text> m_TimerText, m_LevelText, m_KillText;

    // Weapon slot icons
    std::array<std::shared_ptr<Image>, 6> m_WeaponIcons;
};
```

**Bar rendering**: Scale the fill image's X transform to represent percentage:
```cpp
float hpPercent = character.GetHP() / character.GetMaxHP();
m_HPBarFill->m_Transform.scale.x = hpPercent * barWidth;
```

### Z-Index
All HUD elements at z-index `100` (above all game entities).

---

## Stage 5.3: Level-Up UI

### Layout
When LEVEL_UP state activates, display 4 cards:

```
┌──────────────────────────────────┐
│         LEVEL UP!                │
│                                  │
│  ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐  │
│  │ ICON │ │ ICON │ │ ICON │ │ ICON │  │
│  │ Whip │ │Garlic│ │Crown │ │Wings │  │
│  │ Lv.3 │ │ NEW  │ │ Lv.2 │ │ NEW  │  │
│  │      │ │      │ │      │ │      │  │
│  │+5 dmg│ │AoE   │ │+8%XP │ │+10%  │
│  │      │ │around│ │      │ │speed │  │
│  └──────┘ └──────┘ └──────┘ └──────┘  │
└──────────────────────────────────┘
```

### Card Data
```cpp
struct LevelUpCard {
    std::string id;
    std::string name;
    std::string description;  // from JSON level_stats or passive description
    int currentLevel;         // 0 = NEW
    bool isWeapon;
    std::shared_ptr<Image> icon;
};
```

### Interaction
- Mouse hover highlights card (scale up slightly, border glow)
- Mouse click selects → apply upgrade → return to PLAYING
- Keyboard 1-4 for quick select

### Animation
- Cards slide in from bottom (staggered, ~100ms apart)
- Selected card scales up, others fade out
- Brief flash effect, then resume

---

## Stage 5.4: Main Menu

### Layout
```
┌─────────────────────────────────┐
│                                 │
│     P T S D   S U R V I V O R S│
│                                 │
│         [ START GAME ]          │
│                                 │
│                                 │
│                                 │
│    (background: animated map    │
│     with enemies walking)       │
│                                 │
└─────────────────────────────────┘
```

### Features
- Animated background (camera slowly panning over a map with idle enemies)
- Title text (large, possibly animated)
- "Start Game" button → CHARACTER_SELECT
- Background music plays

### Character Select
- Grid of 11 character portraits
- Hover shows name + starting weapon + description
- Click selects → MAP_SELECT

### Map Select
- 3 map thumbnails (Forest, Library, Tower)
- Hover shows name
- Click selects → PLAYING

---

## Stage 5.5: Pause Menu

### Trigger
- ESC key or pause button
- Game freezes (Manager::Update() not called)
- Semi-transparent dark overlay

### Layout
```
┌─────────────────────┐
│      PAUSED         │
│                     │
│   [ RESUME ]        │
│   [ QUIT   ]        │
│                     │
│   Stats:            │
│   Power: 120%       │
│   Speed: 110%       │
│   Area:  130%       │
│                     │
│   Weapons:          │
│   🗡 Whip Lv.5     │
│   🔪 Knife Lv.3    │
│                     │
│   Passives:         │
│   💪 Spinach Lv.3  │
│   ❤️ Hollow Heart 2 │
└─────────────────────┘
```

### Implementation
- Dark overlay: fullscreen `Image` with alpha ~0.7
- Stat summary reads from `Character::GetModifiers()`
- Resume: return to PLAYING state
- Quit: return to MAIN_MENU

---

## Stage 5.6: Game Over & Results

### Trigger
- Character HP reaches 0 (and no revivals left)
- OR timer reaches 30:00 (victory)

### Game Over Screen
```
┌─────────────────────────────────┐
│                                 │
│          GAME OVER              │
│     (or: YOU SURVIVED!)         │
│                                 │
│   Time:  12:34                  │
│   Level: 23                     │
│   Kills: 1,847                  │
│   Coins: 342                    │
│                                 │
│   Weapons:                      │
│   🗡 Bloody Tear (Evo)  Lv.8   │
│   🔪 Thousand Edge (Evo) Lv.8  │
│   🪓 Axe              Lv.5     │
│                                 │
│       [ MAIN MENU ]             │
└─────────────────────────────────┘
```

### Implementation
- Freeze game state
- Fade to dark overlay
- Display stats summary
- "Main Menu" button → MAIN_MENU

---

## Stage 5.7: Chest Opening UI

### Trigger
- Boss killed → treasure chest drops
- Player walks over chest → CHEST_OPEN state

### Layout
- Chest sprite animation (closed → opening → open)
- 1-5 reward items revealed sequentially
- Evolution check: if weapon + passive match → show evolution item with special VFX

### Implementation
```cpp
class ChestUI {
public:
    void Open(const std::vector<std::string>& rewards,
              const std::string& evolution = "");
    void Update(float dt);
    void Draw();
    bool IsDone() const;

private:
    std::vector<LevelUpCard> m_Rewards;
    int m_RevealIndex = 0;
    float m_RevealTimer = 0;
    bool m_HasEvolution;
};
```

---

## Stage 5.8: Damage Numbers

### Floating damage text above enemies when hit

```cpp
class DamageNumber {
public:
    void Spawn(const glm::vec2& position, int damage, bool isCrit);
    void Update(float dt);
    void Draw();
    bool IsExpired() const;

private:
    std::shared_ptr<Text> m_Text;
    glm::vec2 m_Position;
    float m_Lifetime = 0.5f;  // seconds
    float m_Age = 0;
    float m_FloatSpeed = 50.0f;  // pixels/sec upward
};
```

- White text for normal hits, yellow for crits
- Float upward and fade out over 0.5s
- Object-pooled (reuse text objects)

---

## File Summary

| New File | Purpose |
|---|---|
| `include/Game/UI/HUD.hpp` / `.cpp` | Health, XP, timer, weapon slots |
| `include/Game/UI/LevelUpUI.hpp` / `.cpp` | Card selection |
| `include/Game/UI/MainMenu.hpp` / `.cpp` | Title screen, char/map select |
| `include/Game/UI/PauseMenu.hpp` / `.cpp` | Pause overlay |
| `include/Game/UI/GameOverUI.hpp` / `.cpp` | Results screen |
| `include/Game/UI/ChestUI.hpp` / `.cpp` | Chest opening |
| `include/Game/UI/DamageNumber.hpp` / `.cpp` | Floating damage text |
| `include/Game/UI/UICommon.hpp` | Shared: Button, Panel, ProgressBar helpers |

| Modified File | Change |
|---|---|
| `App.hpp` / `App.cpp` | Full state machine refactor |
| `Manager.hpp` / `Manager.cpp` | Expose data for HUD, handle state transitions |
