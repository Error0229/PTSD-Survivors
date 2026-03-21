# Phase 6: Content & Balance

**Goal**: Ship-quality gameplay — all characters, maps, enemies, and balanced difficulty curve.

**Prerequisite**: Phase 5 complete (UI and game systems functional).

**Reference source**: `C:\Users\Tim\Desktop\OOPL-VS` — for exact stat values, sprite mappings, and wave timing.
- `Resources/player_data.csv` — character stats, sprite names, starting weapons
- `Resources/enemy_stats.csv` — HP, power, speed, resistances, XP values for all 83 enemies
- `Resources/weapon_stats.csv` — damage, cooldown, scaling per level for all 21 weapons
- `Resources/wave/` — all 9 wave CSV files (3 maps × enemy/boss/swarm)
- `Source/Game/VSclass/UI.h/.cpp` — original UI layout, damage numbers, icon rendering
- `Source/Game/mygame_run.cpp` — game loop, balance tuning constants
- `Source/Game/mygame_initialize.cpp` — menu flow, character/map selection
- Sprite assets: `Resources/` — numbered PNGs, map tiles, UI elements

---

## Stage 6.1: Character Porting

### 11 Characters from OOPL-VS

| Character | Starting Weapon | Unique Trait | Sprite Frames |
|---|---|---|---|
| Antonio Belpaese | Whip | +10% damage per 10 levels | 4 |
| Imelda Belpaese | Magic Wand | +10% XP per 5 levels | 4 |
| Pasqualina Belpaese | Runetracer | +10% proj speed per 10 levels | 4 |
| Gennaro Belpaese | Knife | +1 projectile at level 6 | 4 |
| Arca Ladonna | Fire Wand | -5% cooldown per 10 levels | 5 |
| Lama Ladonna | Axe | +10% might, +5% move speed per 10 levels | 4 |
| Poe Ratcho | Garlic | -8 max HP, +25% pickup range, +30 move speed | 5 |
| Suor Clerici | Santa Water | +0.5 HP/s, +50% duration, -50% move speed | 4 |
| Dommario | King Bible | +40% duration, +40% speed, -40% move speed | 5 |
| Krochi Freetto | Cross | +1 revival, +30% move speed | 4 |
| Dog Meeo | Heaven Sword (evo) | Starts with evo weapon, +20% move speed | 5 |

### Tasks per Character
1. Extract sprite sheet from OOPL-VS `resources/Sprite/`
2. Verify animation frames match frame count
3. Add entry to `characters.json` with base stats + level bonuses
4. Test: spawn as character, verify starting weapon, verify sprite/animation
5. Verify level-up stat bonuses apply correctly

### Sprite Mapping
OOPL-VS sprites are numbered PNGs (e.g., `00.png` through `99.png`). Need to map character names to sprite IDs:
- Reference `player_data.csv` column `textureName` for the sprite prefix
- Each character has `animation_frames` count (4 or 5 walk cycle frames)

---

## Stage 6.2: Map Porting

### 3 Maps from OOPL-VS

| Map | Sprite | Enemies | Special |
|---|---|---|---|
| Mad Forest | `01_Forest.png` | Bats, flowers, ghosts, zombies | Standard open field |
| Inlaid Library | `02_Hills.png` | Muds, ectos, mummies, medusas | Narrow corridors |
| Dairy Plant | `03_Mountains.png` | Devils, harpies, dragons, imps | Open with obstacles |

### Map Implementation

**Current**: `Map` class renders a single tiled background.

**Target**:
1. Infinite scrolling tile map (character moves, map repeats)
2. Each map has a tile texture (repeating pattern)
3. Spawn edges defined per map (which screen edges enemies enter from)
4. Optional obstacles (collision geometry)

### Tile Rendering
```cpp
void Map::Draw() {
    // Calculate visible tile range based on camera position
    int startX = (int)(cameraX / tileW) - 1;
    int startY = (int)(cameraY / tileH) - 1;
    int endX = startX + (screenW / tileW) + 3;
    int endY = startY + (screenH / tileH) + 3;

    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            // Draw tile at world position (x * tileW, y * tileH)
        }
    }
}
```

### Spawn Edges (from OOPL-VS)
- **Mad Forest**: All 4 edges
- **Inlaid Library**: Top + bottom only
- **Dairy Plant**: Left + right only

---

## Stage 6.3: Enemy Porting

### Priority Order
Port enemies in wave order — start with Stage 1 (Mad Forest), then Stage 2, then Stage 3.

**Stage 1 enemies (22 types)**:
BAT1-5, FLOWER1-2, GHOST, ZOMBIE1-3, MUDNAN1-2, SKELETON2/3/5/6, WEREWOLF, XLBAT, XLFLOWER, XLMANTIS, XLMUMMY, SWARM_BAT, SWARM_GHOST

**Stage 2 enemies (25 types)**:
MUD, ECTO1-2, MUMMY, DULL1-2, MEDUSA1-2, BUER, WITCH1-2, SKULL2, XLMEDUSA, SWARM_SHADERED, SWARM_SKULL2, SKULLINO, SKULOROSSO, SKELANGUE, + boss variants

**Stage 3 enemies (30 types)**:
DULL0, DEVIL1-2, BAT6, GHOST2, IMP, HARPY, XLDRAGON1-2, XLMAGIO, SKELEWING, XLARMOR1-2, XLCOCKATRICE, XLCHIMERA, XLGOLEM3-4, + boss variants

**+ DEATH** (final boss, minute 30)

### Tasks per Enemy Type
1. Extract sprite from OOPL-VS resources (map sprite number → enemy name)
2. Add entry to `enemies.json` with stats from `enemy_stats.csv`
3. Assign AI behavior (chase / swarm / wall / sin-wave / boss)
4. Assign animation (frame count, frame time)
5. Test: verify spawning, movement, death, XP drop

### Boss Specifics
- Much higher HP + damage than regular enemies
- Drops treasure chest on death
- Chest may trigger weapon evolution
- Some bosses have unique movement (slower but tankier)

---

## Stage 6.4: Wave Data Porting

### Convert OOPL-VS wave CSVs → JSON

3 maps × 3 wave types = 9 CSV files → 3 JSON files

**For each stage**:
1. Parse `stageN_wave_enemy.csv` → `waves/stageN.json` enemies array
2. Parse `stageN_wave_boss.csv` → `waves/stageN.json` bosses array
3. Parse `stageN_wave_swarm.csv` → `waves/stageN.json` swarms array
4. Verify time ranges (0-30 minutes)
5. Verify all referenced enemy types exist in `enemies.json`

### Wave Timing Overview (Stage 1 example)
- **0:00-1:00**: Bats only, slow spawn
- **1:00-3:00**: Bats + flowers, faster
- **3:00**: First swarm attack
- **5:00**: First boss (XL Mantis)
- **5:00-10:00**: Ghosts, zombies mix in
- **10:00**: Second boss
- **10:00-20:00**: Escalating mix of all enemy types
- **20:00-29:00**: Everything at max intensity
- **30:00**: DEATH spawns (effectively unkillable, forces game end)

---

## Stage 6.5: Balance Tuning

### Weapon Balance

Compare against OOPL-VS values in `weapon_stats.csv`:

| Weapon | Damage | Cooldown | Key Scaling |
|---|---|---|---|
| Whip | 10 | 1.15s | +5 dmg per 2 levels |
| Magic Wand | 10 | 1.35s | Targets nearest |
| Knife | 8 | 0.3s | Fast fire, low pierce |
| Axe | 20 | 1.65s | High area scaling |
| Cross | 15 | 1.2s | Boomerang + pierce |
| King Bible | 10 | 1.0s | Orbit + amount scaling |
| Fire Wand | 20 | 1.5s | Random target, high dmg |
| Garlic | 5 | 0.5s | AoE, fast tick |
| Santa Water | 10 | 3.0s | Area denial |
| Runetracer | 10 | 3.0s | Long duration, bounces |

### Enemy Scaling
- HP scales with player level (for most enemies): `hp = base_hp * (1 + player_level * 0.1)`
- Curse modifier affects: enemy HP, speed, damage, spawn rate
- Boss HP scales more aggressively

### Difficulty Curve Targets
- **Minutes 0-5**: Player should feel increasingly powerful
- **Minutes 5-10**: Challenge ramps, first tough moments
- **Minutes 10-15**: Player needs 3-4 weapons to survive
- **Minutes 15-20**: Screen should be chaotic, evolved weapons needed
- **Minutes 20-25**: Peak challenge, screen full of enemies
- **Minutes 25-30**: Survival endurance, DEATH approaches

### Tuning Process
1. Start with exact OOPL-VS values
2. Playtest each weapon individually (does it feel right?)
3. Playtest weapon combinations (are any broken / useless?)
4. Playtest full 30-minute runs per map
5. Adjust numbers in JSON (no recompile needed)

---

## Stage 6.6: Audio

### Music
- Main menu BGM
- Per-map gameplay BGM (OOPL-VS has `bgm_elrond_*` tracks)
- Boss encounter BGM
- Game over BGM

### Sound Effects
- Weapon fire sounds (per weapon type)
- Enemy hit / death sounds
- Player hit sound
- Level-up jingle
- Chest open sound
- Pickup collect sound

### Format Optimization
Current audio is WAV (large). Convert to:
- **MP3** for music (lossy, good compression)
- **WAV** for short SFX (low latency) or **MP3** if size is a concern

SDL2_mixer on Emscripten supports MP3 natively.

---

## Stage 6.7: Asset Optimization

### Target: < 20 MB total download

**Current estimated sizes**:
- Sprite PNGs: ~5-10 MB (10,000+ individual files in OOPL-VS)
- Audio: ~10-20 MB (WAV)
- WASM binary: ~2-5 MB
- JSON data: < 100 KB

### Optimization Steps

1. **Texture atlases** (from Phase 3) reduce file count and improve compression
2. **Audio conversion**: WAV → MP3 (10:1 compression ratio)
3. **Sprite dedup**: Many OOPL-VS sprites are duplicates or unused — audit and remove
4. **PNG optimization**: `optipng` or `pngcrush` on all sprite sheets
5. **WASM optimization**: `-O2` or `-Os` build flag, `--closure 1` for JS minification
6. **Gzip/Brotli**: Server-side compression (the hosting server should serve `.wasm` with Content-Encoding)

### Size Budget

| Category | Budget |
|---|---|
| WASM + JS | 3 MB |
| Texture atlases | 8 MB |
| Audio (MP3) | 6 MB |
| JSON + fonts | 1 MB |
| **Total** | **< 18 MB** |

---

## Stage 6.8: Deployment

### Build
```bash
emcmake cmake -B build-web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web
```

### Output Files
- `Sample.html` → rename to `index.html`
- `Sample.js` → game loader
- `Sample.wasm` → compiled game
- `Sample.data` → preloaded assets

### Hosting
Deploy to `error0229.github.io/secret/`:
1. Copy build output to `D:\Error0229.github.io\secret\`
2. Customize `index.html` (remove Emscripten default shell, add custom loading screen)
3. Test locally: `python -m http.server 8080`
4. Push to GitHub Pages

### Easter Egg Integration
The game is triggered by clicking the hidden "???" node in the portfolio's D3.js skill network graph. The secret page already has a blueprint theme (from the BoxedWine attempt). Update it to load the WASM game instead.

### Performance Targets
- **Load time**: < 3 seconds on broadband
- **FPS**: Stable 60 FPS during gameplay
- **Memory**: < 256 MB WASM heap
- **Download**: < 20 MB total

### Browser Compatibility
Test on:
- Chrome (primary target)
- Firefox
- Safari (WebGL 2.0 support varies)
- Edge (Chromium-based, should match Chrome)
- Mobile Chrome/Safari (bonus, not required)
