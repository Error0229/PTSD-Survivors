# Design Patterns Guide

Use patterns where they solve real problems. Don't add ceremony.

## Entity-Component (Mixins) — Keep

The `Physical`, `Animated`, `Mirrored` mixin classes compose behavior without deep inheritance. Extend this for new behaviors (`Damageable`, `Expirable`).

```cpp
class Enemy : public GameObject, Physical, Animated, Mirrored { ... };
```

## Object Pool — Keep

`ObjectPool<T>` for enemies and projectiles avoids allocation churn. Correct for a survivors game where hundreds of entities spawn/despawn per second.

## Singleton (Manager) — Keep, Keep Simple

`Manager` owns the game world. Don't fight the pattern with dependency injection for a single-binary game.

## State Pattern — Expand

Current `App::State` enum is a start. Expand to:
- **Game states**: Menu → Playing → Paused → GameOver → Results
- **Enemy AI**: Idle → Chasing → Attacking → Dying (proper state machine, not boolean flags)
- **Character**: Alive → Invincible → Dead

## Strategy Pattern — For Weapons/Projectiles

Each weapon type defines its firing pattern as a strategy. Don't duplicate weapon classes (current `Vampirica` = copy of `Whip`).

```cpp
class ProjectileBehavior {
public:
    virtual void Update(Projectile& p, float dt) = 0;
};

class LinearBehavior : public ProjectileBehavior { ... };
class OrbitBehavior : public ProjectileBehavior { ... };
class BoomerangBehavior : public ProjectileBehavior { ... };
```

Weapons become data-driven — define behavior type + stats in JSON, not in separate C++ classes.

## Observer — For Game Events

Lightweight event system for decoupled communication:
- Enemy dies → spawn XP pickup, update kill count
- Player levels up → show level-up UI
- Wave completes → spawn boss

Keep it simple — `std::function` callbacks, not a full event bus.

## Factory + Data-Driven Creation — For Content

Load definitions from JSON. Expand the existing `Resource` class:

```json
{
  "whip": {
    "damage": 10,
    "cooldown": 1.5,
    "projectile": "whip_slash",
    "behavior": "melee_swing",
    "evolution": { "requires": "hollow_heart", "becomes": "bloody_tear" }
  }
}
```

One `Weapon` class configured by data, not 11 weapon subclasses.
