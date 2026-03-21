# Phase 4: Complete Game Systems

**Goal**: Feature parity with the original OOPL-VS game.

**Prerequisite**: Phase 3 complete (60 FPS rendering in browser).

**Reference source**: `C:\Users\Tim\Desktop\OOPL-VS` — the original Win32/MFC Vampire Survivors clone. Key files:
- `Source/Game/VSclass/Weapon.h/.cpp` — weapon enum, firing logic, evolution pairs
- `Source/Game/VSclass/Projectile.h/.cpp` — projectile movement, lifetime, collision
- `Source/Game/VSclass/Enemy.h/.cpp` — enemy AI, spawn positions, scaling
- `Source/Game/VSclass/EnemyFactory.h/.cpp` — wave spawning, boss/swarm logic
- `Source/Game/VSclass/Player.h/.cpp` — stats, level-up, XP curve, modifier application
- `Source/Game/VSclass/Passive.h/.cpp` — passive effects, stacking, pricing
- `Source/Game/VSclass/Pickup.h/.cpp` — XP gems, coins, chests, magnet behavior
- `Resources/weapon_stats.csv` — all weapon base stats + evolution stats (21 rows)
- `Resources/enemy_stats.csv` — all 83 enemy definitions
- `Resources/player_data.csv` — all 11 character definitions
- `Resources/item_id.csv` — master ID enum for all items
- `Resources/wave/stage[1-3]_wave_*.csv` — wave timing data (enemy/boss/swarm per map)

---

## Stage 4.1: Data-Driven Foundation (JSON)

Before building game systems, convert OOPL-VS CSV data → JSON and build a loader.

### Data Files to Create

**`resources/Data/weapons.json`** — 10 base weapons + 11 evolutions:
```json
{
  "WHIP": {
    "id": 0,
    "damage": 10, "speed": 1.0, "area": 1.0,
    "rarity": 100, "amount": 1, "duration": 0.35,
    "pierce": 0, "cooldown": 1.15, "proj_interval": 0.1,
    "hitbox_delay": 0.0, "knock_back": 0, "pool_limit": 30,
    "chance": 0, "crit_multi": 0, "block_by_wall": false,
    "behavior": "melee_swing",
    "max_level": 8,
    "level_stats": {
      "2": { "amount": 1 },
      "3": { "damage": 5 },
      "4": { "area": 0.1 },
      "5": { "amount": 1 },
      "6": { "damage": 5 },
      "7": { "area": 0.1 },
      "8": { "damage": 5 }
    },
    "evolution": { "requires": "HOLLOW_HEART", "becomes": "BLOODY_TEAR" }
  }
}
```

**`resources/Data/enemies.json`** — 83 enemy types:
```json
{
  "BAT1": {
    "id": 1,
    "hp_max": 5, "power": 5, "move_speed": 80,
    "knockback": 2, "kb_max": 2,
    "res_freeze": false, "res_kill": false, "res_debuff": false,
    "xp_value": 1, "hp_scales": true, "spawn_limit": 0,
    "sprite": "bat1", "animation_frames": 4, "frame_time": 100
  }
}
```

**`resources/Data/characters.json`** — 11 characters:
```json
{
  "ANTONIO": {
    "name": "Antonio Belpaese",
    "sprite": "Antonio", "animation_frames": 4,
    "starting_weapon": "WHIP",
    "base_stats": {
      "max_hp": 120, "armor": 1, "regen": 0,
      "move_speed": 1.0, "power": 1.0, "area": 1.0,
      "speed": 1.0, "duration": 1.0, "amount": 0,
      "luck": 1.0, "growth": 1.0, "greed": 1.0,
      "curse": 1.0, "magnet": 0, "revivals": 0
    },
    "level_bonuses": {
      "2": { "power": 0.1 },
      "10": { "power": 0.1 },
      "20": { "power": 0.1 }
    }
  }
}
```

**`resources/Data/passives.json`** — 16+ passive items:
```json
{
  "SPINACH": {
    "id": 63, "rarity": 100, "max_level": 5,
    "effect": { "stat": "power", "per_level": 0.1, "type": "additive" },
    "description": "Raises inflicted damage by 10%."
  }
}
```

**`resources/Data/waves/stage1.json`** — Wave definitions per map:
```json
{
  "enemies": [
    { "time_min": 0, "pool": [{"type": "BAT1", "weight": 80}, {"type": "FLOWER1", "weight": 20}], "min_count": 5, "interval_ms": 2000 },
    { "time_min": 1, "pool": [{"type": "BAT1", "weight": 60}, {"type": "GHOST", "weight": 40}], "min_count": 8, "interval_ms": 1800 }
  ],
  "bosses": [
    { "time_min": 5, "type": "BOSS_XLMANTIS", "chest_evo_chance": 0.2 }
  ],
  "swarms": [
    { "time_min": 3, "type": "SWARM_BAT", "amount": 20, "interval_ms": 100, "duration_ms": 5000, "pattern": "charge" }
  ]
}
```

### JSON Loader

**Modify**: `src/Game/Resource.cpp`

Use `nlohmann/json` (already a dependency from Phase 1) to load all data files at `Resource::Initialize()`:

```cpp
void Resource::Initialize() {
    auto weapons = LoadJson("resources/Data/weapons.json");
    auto enemies = LoadJson("resources/Data/enemies.json");
    auto characters = LoadJson("resources/Data/characters.json");
    auto passives = LoadJson("resources/Data/passives.json");
    // Store in static maps, keyed by string ID
}
```

Replace current `test.json` loading with the new structured format.

---

## Stage 4.2: Weapon System Overhaul

### Current State
- 2 weapons: `Whip` and `Vampirica` (which is a Whip copy)
- Each weapon is a separate C++ class inheriting `Weapon`
- Each projectile is a separate C++ class inheriting `Projectile`

### Target
- 10 base weapons + 11 evolutions, all driven by JSON
- **One `Weapon` class**, configured by data
- **One `Projectile` class**, behavior determined by Strategy pattern

### Projectile Behavior Strategies

**New file**: `include/Game/Projectile/Behavior.hpp`

```cpp
class ProjectileBehavior {
public:
    virtual ~ProjectileBehavior() = default;
    virtual void Start(Projectile& p, const glm::vec2& origin, const glm::vec2& target) = 0;
    virtual void Update(Projectile& p, float dt) = 0;
};
```

**Implementations** (one file each in `src/Game/Projectile/Behavior/`):

| Behavior | Used By | Logic |
|---|---|---|
| `LinearBehavior` | Knife, Fire Wand, Magic Wand | Move in straight line toward target |
| `MeleeSwingBehavior` | Whip, Bloody Tear | Offset from character, play animation, no movement |
| `OrbitBehavior` | King Bible, Vespers | Orbit around character at fixed radius |
| `BoomerangBehavior` | Cross, Heaven Sword | Move toward target, return to origin |
| `AoEBehavior` | Garlic, Vortex | Static area at character position, damages over time |
| `PoolBehavior` | Holy Water, Bora | Drop at position, create damage zone |
| `BounceBehavior` | Runetracer, Rocher | Bounce off screen edges, persist for duration |
| `RadialBehavior` | Axe (up arc) | Fire upward with gravity-like arc |

### Weapon Firing Logic

Refactor `Weapon::Update()`:
```cpp
void Weapon::Update(float dt) {
    m_CooldownTimer -= dt;
    if (m_CooldownTimer > 0) return;
    m_CooldownTimer = m_Stats.cooldown * m_Modifiers.cooldown;

    int amount = m_Stats.amount + m_Modifiers.amount;
    for (int i = 0; i < amount; i++) {
        auto* proj = Resource::GetProjectile(m_Id);
        proj->SetBehavior(m_BehaviorFactory()); // creates LinearBehavior, etc.
        proj->SetStats(m_Stats, m_Modifiers);
        proj->SetDelay(i * m_Stats.proj_interval);

        glm::vec2 target = ComputeTarget(i); // nearest enemy, cursor direction, etc.
        proj->GetBehavior()->Start(*proj, m_Owner->GetPosition(), target);

        Manager::AddProjectile(proj);
    }
}
```

### Target Selection

| Weapon | Targeting |
|---|---|
| Whip | Left/right of character (alternating) |
| Magic Wand | Nearest enemy |
| Knife | Faced direction (cursor) |
| Axe | Upward arc |
| Cross | Nearest enemy (boomerang) |
| King Bible | N/A (orbit) |
| Fire Wand | Random enemy |
| Garlic | N/A (AoE at self) |
| Holy Water | Random nearby position |
| Runetracer | Random direction |

### Files to Create/Modify

| Action | File |
|---|---|
| Create | `include/Game/Projectile/Behavior.hpp` (base + all strategies) |
| Create | `src/Game/Projectile/Behavior/Linear.cpp` |
| Create | `src/Game/Projectile/Behavior/MeleeSwing.cpp` |
| Create | `src/Game/Projectile/Behavior/Orbit.cpp` |
| Create | `src/Game/Projectile/Behavior/Boomerang.cpp` |
| Create | `src/Game/Projectile/Behavior/AoE.cpp` |
| Create | `src/Game/Projectile/Behavior/Pool.cpp` |
| Create | `src/Game/Projectile/Behavior/Bounce.cpp` |
| Create | `src/Game/Projectile/Behavior/Radial.cpp` |
| Modify | `include/Game/Projectile/Projectile.hpp` — add `m_Behavior` member |
| Modify | `src/Game/Projectile/Projectile.cpp` — delegate Update to behavior |
| Modify | `include/Game/Weapon/Weapon.hpp` — generic firing, remove subclasses |
| Modify | `src/Game/Weapon/Weapon.cpp` — data-driven stats, target selection |
| Delete | `include/Game/Weapon/Whip.hpp`, `Vampirica.hpp` |
| Delete | `src/Game/Weapon/Whip.cpp`, `Vampirica.cpp` |
| Delete | `include/Game/Projectile/Whip.hpp`, `Vampirica.hpp` |
| Delete | `src/Game/Projectile/Whip.cpp`, `Vampirica.cpp` |

---

## Stage 4.3: Enemy System Expansion

### Current State
- Single `Enemy` class with basic chase behavior
- Stats loaded from `test.json`
- Object-pooled via `Resource`

### Target
- 83 enemy types, all data-driven from `enemies.json`
- Enemy AI state machine
- Swarm/boss/wall/sin-wave movement patterns

### Enemy AI States

**New file**: `include/Game/Enemy/EnemyState.hpp`

```cpp
enum class EnemyAIState { IDLE, CHASING, ATTACKING, STUNNED, DYING };

class EnemyAI {
public:
    virtual ~EnemyAI() = default;
    virtual void Update(Enemy& enemy, float dt, const glm::vec2& playerPos) = 0;
};

class ChaseAI : public EnemyAI { ... };     // Standard pursuit
class SwarmAI : public EnemyAI { ... };      // Charge from edge
class WallAI : public EnemyAI { ... };       // Ellipse around player
class SinWaveAI : public EnemyAI { ... };    // Undulating approach
class BossAI : public EnemyAI { ... };       // Chase + special attacks
```

### Enemy Stun System (already partially implemented)
- On hit: reverse direction for 120ms
- Boss enemies: reduced knockback
- Resistance flags: `res_freeze`, `res_kill`, `res_debuff`

### Enemy Spawning

**Refactor**: `Manager::EnemyGen()` → `WaveManager`

**New class**: `Game::WaveManager`

```cpp
class WaveManager {
public:
    void Load(const std::string& stageFile);  // loads stage JSON
    void Update(float dt, float gameTime);     // spawns based on time

private:
    struct WaveEntry { float time; std::vector<WeightedEnemy> pool; int minCount; float interval; };
    struct BossEntry { float time; std::string type; float evoChance; };
    struct SwarmEntry { float time; std::string type; int amount; float interval; std::string pattern; };

    std::vector<WaveEntry> m_Enemies;
    std::vector<BossEntry> m_Bosses;
    std::vector<SwarmEntry> m_Swarms;

    float m_SpawnTimer = 0;
    int m_CurrentWaveIndex = 0;

    glm::vec2 RandomSpawnPosition(int mapId);
};
```

### Spawn Position Logic (from OOPL-VS)
- 4 edges of screen + padding
- Random position along edge
- Different maps have different valid edges

---

## Stage 4.4: Passive Item System

### Current State
- `Passive` class exists but is minimal
- Stats loaded from `test.json`

### Target
- 16 passive types, all data-driven
- Passives modify weapon/character stats via coefficients
- Passive upgrade available at level-up

### Stat Modifier System

**New**: Character/Weapon stats are base × (1 + passive_bonus)

```cpp
struct StatModifiers {
    float power = 0;       // % bonus to damage
    float armor = 0;       // flat damage reduction
    float maxHealth = 0;   // % bonus to max HP
    float regen = 0;       // flat HP/s
    float cooldown = 0;    // % reduction (negative = faster)
    float area = 0;        // % bonus to hitbox size
    float speed = 0;       // % bonus to projectile speed
    float duration = 0;    // % bonus to duration
    int   amount = 0;      // flat extra projectiles
    float moveSpeed = 0;   // % bonus to move speed
    float magnet = 0;      // flat extra pickup range
    float luck = 0;        // % bonus
    float growth = 0;      // % bonus XP
    float greed = 0;       // % bonus coins
    int   revival = 0;     // flat extra lives
    float curse = 0;       // % bonus enemy stats (double-edged)
};
```

**Character** owns a `StatModifiers` that accumulates all passive bonuses:
```cpp
void Character::RecalculateModifiers() {
    m_Modifiers = {};  // reset
    for (auto& passive : m_Passives) {
        passive->ApplyTo(m_Modifiers);
    }
    // Notify weapons to recalculate
    for (auto& weapon : m_Weapons) {
        weapon->UpdateModifiers(m_Modifiers);
    }
}
```

---

## Stage 4.5: Pickup System

### Types

| Pickup | Source | Effect |
|---|---|---|
| XP Gem (blue) | Enemy death | +1 XP |
| XP Gem (green) | Elite death | +3 XP |
| XP Gem (red) | Boss death | +10 XP |
| Coin | Enemy death (luck-based) | +1 coin |
| Treasure Chest | Boss death | 1-5 weapon/passive rewards |
| Floor Chicken | Random spawn | Heal 30 HP |
| Rosary | Rare spawn | Kill all on-screen enemies |
| Vacuum | 5-minute mark | Collect all on-screen pickups |

### Implementation

**New class**: `Game::Pickup`

```cpp
class Pickup : public GameObject, Physical, Animated {
public:
    enum class Type { XP_SMALL, XP_MED, XP_LARGE, COIN, CHEST, CHICKEN, ROSARY, VACUUM };

    void Start(Type type, const glm::vec2& position);
    void Update(float dt, const glm::vec2& playerPos, float magnetRange);
    bool IsCollected() const;

    Type GetType() const;
    int GetValue() const;

private:
    Type m_Type;
    int m_Value;
    bool m_Collected = false;
    bool m_Attracted = false;  // within magnet range, moving toward player
};
```

Object-pooled like enemies/projectiles.

**Magnet behavior**: When pickup enters `magnetRange` of player, it accelerates toward player. On contact, collected.

---

## Stage 4.6: Level-Up System

### XP Curve (from OOPL-VS)
- Levels 1-19: base 5 + 10/level
- Levels 20-39: base 205 + 13/level (milestone boost at 20)
- Levels 40+: base 475 + 16/level (milestone boost at 40)

### Level-Up Flow
1. XP reaches threshold → trigger level-up
2. Pause game
3. Present 4 random choices (weapons or passives)
4. Player selects one
5. Apply upgrade, resume game

### Choice Generation
```cpp
std::vector<LevelUpChoice> GenerateChoices(int count = 4) {
    std::vector<LevelUpChoice> pool;

    // New weapons (not yet acquired, not at weapon cap of 6)
    if (m_Weapons.size() < 6) {
        for (auto& [id, data] : weaponDefs) {
            if (!Have(id)) pool.push_back({id, "weapon", "new"});
        }
    }

    // Weapon level-ups (acquired, not max level)
    for (auto& weapon : m_Weapons) {
        if (!weapon->IsMaxLevel())
            pool.push_back({weapon->ID(), "weapon", "upgrade"});
    }

    // New passives (not yet acquired, not at passive cap of 6)
    // Passive level-ups (acquired, not max level)
    // ... same pattern ...

    // Weighted random selection (rarity-based)
    return WeightedSample(pool, count);
}
```

---

## Stage 4.7: Evolution System

### Evolution Pairs (from OOPL-VS)

| Weapon | Required Passive | Evolved Form |
|---|---|---|
| Whip | Hollow Heart | Bloody Tear |
| Magic Wand | Empty Tome | Holy Wand |
| Knife | Bracer | Thousand Edge |
| Axe | Candelabrador | Death Spiral |
| Cross | Clover | Heaven Sword |
| King Bible | Spellbinder | Unholy Vespers |
| Fire Wand | Spinach | Hellfire |
| Garlic | Pummarola | Soul Eater |
| Santa Water | Attractorb | La Borra |
| Runetracer | Armor | NO FUTURE |

### Evolution Trigger
- Weapon at max level
- Required passive acquired (any level)
- Player opens a treasure chest (from boss kill)
- Chest checks evolution eligibility → if eligible, evolve

### Implementation
Evolution data is in `weapons.json` under `"evolution"` key. When chest opens:
```cpp
for (auto& weapon : m_Weapons) {
    if (weapon->IsMaxLevel() && weapon->CanEvolve()) {
        auto evoId = weaponDefs[weapon->ID()].evolution.becomes;
        weapon->Evolve(evoId);  // replace stats, animation, behavior
        break;  // one evolution per chest
    }
}
```

---

## Stage 4.8: Collision System Fix

### Current Bug
Projectile-enemy collision is disabled in `Manager.cpp` (line with `continue;`).

### Fix
Re-enable the collision check:
```cpp
// In Manager::Update(), collision section:
for (auto& projectile : m_Projectiles) {
    auto nearbyEnemies = m_QuadTree.QueryCollision(projectile);
    for (auto* enemy : nearbyEnemies) {
        if (projectile->IsCollideWith(*enemy)) {
            enemy->HitBy(projectile, projectile->Get("damage"));
            projectile->OnHit();  // reduce pierce count, etc.
            if (projectile->Get("pierce") <= 0) {
                projectile->MarkOver();
                break;
            }
        }
    }
}
```

### Also Fix: Enemy Width/Height Swap
**Known bug from CLAUDE.md**: `Enemy::Width()` and `Height()` are swapped. Fix in `include/Game/Enemy/Enemy.hpp`.

---

## Estimated Scope

| Stage | New Files | Modified Files | Complexity |
|---|---|---|---|
| 4.1 JSON data | 5 data files | Resource.cpp | Medium |
| 4.2 Weapons | 8-10 behavior files | Weapon, Projectile | High |
| 4.3 Enemies | 5-6 AI files, WaveManager | Enemy, Manager | High |
| 4.4 Passives | 0 | Passive, Character | Medium |
| 4.5 Pickups | 2 (Pickup.h/.cpp) | Manager, Enemy (drops) | Medium |
| 4.6 Level-up | 2 (LevelUpSystem.h/.cpp) | Manager, Character | Medium |
| 4.7 Evolution | 0 | Weapon, LevelUpSystem | Low |
| 4.8 Collision fix | 0 | Manager.cpp, Enemy.hpp | Low |
