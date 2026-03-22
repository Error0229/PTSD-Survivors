# TODOS

## Phase 4 — Completed

### ~~TODO-001~~ ✓ Audit weapon movement patterns
9 behavior types identified. Completed 2026-03-22.

### ~~TODO-002~~ ✓ Wave data (all 3 stages)
All 9 OOPL-VS wave CSVs converted to JSON. 83 unique enemies referenced, 62 available in our data, 21 missing (graceful skip with log warning).

### ~~TODO-003~~ ✓ JSON validation for wave loader
WaveManager::Load() uses safe access with logging. Completed 2026-03-22.

### ~~TODO-004~~ ✓ Level-up fallback for maxed builds
LevelUpSystem::GenerateChoices() falls back to GOLD when pool is empty.

### ~~TODO-005~~ ✓ Wave data for stages 2 and 3
Converted from OOPL-VS CSVs with numeric→name mapping via enemy_stats.csv.

### ~~TODO-008~~ ✓ Chest opening flow
Boss death → chest drop → TryEvolveWeapon() → fallback to level-up.

## Phase 5/6 — Deferred

### TODO-006: Level-up choice UI
Currently auto-picks first choice. Phase 5 will add card selection UI with descriptions.

### TODO-007: Pickup sprites
Pickups are logic-only (invisible). Need XP gem, coin, chest, chicken, rosary, vacuum sprites.

### TODO-009: Stat name canonicalization (Codex issue #1)
`"power"` vs `"damage"`, `"interval"` vs `"coolDown"` naming inconsistency.
Not a bug but a maintenance hazard. Defer until Phase 6 data finalization.

### TODO-010: 21 missing enemy definitions
DEATH, FLOWER1, FLOWER2, ZOMBIE1, ZOMBIE2, MUDNAN1, MUDNAN2, SKULL2, SKELETON5, SKELETON6, and 11 boss/swarm types not in our enemy data. Need to add these definitions to v1.3.100_ENEMY_DATA.txt in Phase 6.
