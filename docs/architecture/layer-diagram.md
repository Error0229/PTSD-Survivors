# Architecture Layers

```
┌──────────────────────────────────────────────┐
│                  main.cpp                     │
│        Entry point + platform ifdef           │
│   (emscripten_set_main_loop vs native loop)   │
├──────────────────────────────────────────────┤
│                   App.cpp                     │
│             Game state machine                │
│     Menu → Playing → Paused → GameOver        │
├──────────────┬───────────────────────────────┤
│   Core/      │          Util/                 │
│  Rendering   │   Framework abstractions       │
│  Context     │   Image, Text, Input,          │
│  Shader      │   GameObject, Transform,       │
│  Texture     │   BGM, SFX, Time, Renderer     │
│  SpriteBatch │                                │
├──────────────┴───────────────────────────────┤
│                  Game/                         │
│   Manager, Character, Enemy, Weapon,          │
│   Projectile, Passive, Map, Camera,           │
│   Resource, WaveManager, LevelUpSystem,       │
│   Pickup, UI                                  │
└──────────────────────────────────────────────┘
```

## Dependency Rules

- **Core/** → SDL2, OpenGL/GLES, GLM (nothing else)
- **Util/** → Core/ (never Game/)
- **Game/** → Util/, Core/
- Game logic **never** calls GL or SDL directly

## File Organization

```
src/
├── main.cpp
├── App.cpp
├── Core/           # GL-level rendering primitives
├── Util/           # Framework abstractions (Image, Input, Text, etc.)
└── Game/
    ├── Manager.cpp
    ├── Character.cpp
    ├── Camera.cpp
    ├── Map.cpp
    ├── Resource.cpp
    ├── WaveManager.cpp       # NEW
    ├── LevelUpSystem.cpp     # NEW
    ├── Enemy/
    ├── Weapon/
    │   └── Behavior/         # Projectile movement strategies
    ├── Projectile/
    ├── Passive/
    ├── Pickup/               # NEW
    └── UI/                   # NEW
resources/
├── Sprite/         # PNG sprite sheets
├── Font/           # TTF fonts
├── AudioClip/      # MP3/OGG
└── Data/           # JSON game data
    ├── characters.json
    ├── enemies.json
    ├── weapons.json
    ├── passives.json
    └── waves.json
```
