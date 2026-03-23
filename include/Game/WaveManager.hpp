#ifndef WAVE_MANAGER_HPP
#define WAVE_MANAGER_HPP

#include "pch.hpp"
#include <string>
#include <vector>

namespace Game {

// Spawn request returned to Manager — WaveManager decides WHAT, Manager decides
// WHERE
struct SpawnRequest {
    std::string enemyType;
    bool isBoss = false;
    bool isSwarm = false;
    int swarmAmount = 1;
    float chestEvoChance = 0.0f; // for boss chests
};

class WaveManager {
public:
    void Load(const std::string &stageFile);
    std::vector<SpawnRequest> Update(float dt, float gameTime, int playerLevel);

private:
    // Wave track: enemy spawning (per-minute pools)
    struct WaveEntry {
        float timeMin = 0; // minute this wave starts
        struct WeightedEnemy {
            std::string type;
            float weight = 1.0f;
        };
        std::vector<WeightedEnemy> pool;
        int minCount = 5;        // burst size per spawn tick
        float intervalMs = 2000; // ms between spawn ticks
    };

    // Boss track: timed boss spawns
    struct BossEntry {
        float timeMin = 0;
        std::string type;
        float evoChance = 0;
        bool spawned = false; // only spawn once
    };

    // Swarm track: burst enemy charges
    struct SwarmEntry {
        float timeMin = 0;
        std::string type;
        int amount = 20;
        float intervalMs = 100;
        float durationMs = 5000;
        bool triggered = false;
    };

    // Per-track independent timers (Codex issue #3)
    float m_EnemySpawnTimer = 0;
    int m_CurrentWaveIndex = 0;

    std::vector<WaveEntry> m_Waves;
    std::vector<BossEntry> m_Bosses;
    std::vector<SwarmEntry> m_Swarms;

    // Active swarm state
    struct ActiveSwarm {
        std::string type;
        int remaining;
        float intervalMs;
        float timer;
    };
    std::vector<ActiveSwarm> m_ActiveSwarms;

    std::string PickWeightedEnemy(const WaveEntry &wave) const;
    int FindWaveForTime(float gameTimeMinutes) const;
};

} // namespace Game

#endif // WAVE_MANAGER_HPP
