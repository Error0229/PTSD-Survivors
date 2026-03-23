#include "Game/WaveManager.hpp"
#include "Game/Config.hpp"
#include "Util/Logger.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <random>

namespace Game {

static std::mt19937 &RNG() {
    static std::mt19937 gen(std::random_device{}());
    return gen;
}

void WaveManager::Load(const std::string &stageFile) {
    using json = nlohmann::json;
    std::ifstream file(stageFile);
    if (!file.is_open()) {
        LOG_WARN("Wave data not found: {}", stageFile);
        return;
    }

    auto data = json::parse(file, nullptr, false);
    if (data.is_discarded()) {
        LOG_WARN("Wave data parse error: {}", stageFile);
        return;
    }

    // Load enemy waves
    if (data.contains("enemies") && data["enemies"].is_array()) {
        for (auto &entry : data["enemies"]) {
            WaveEntry wave;
            wave.timeMin = entry.value("time_min", 0.0f);
            wave.minCount = entry.value("min_count", 5);
            wave.intervalMs = entry.value("interval_ms", 2000.0f);

            if (entry.contains("pool") && entry["pool"].is_array()) {
                for (auto &e : entry["pool"]) {
                    WaveEntry::WeightedEnemy we;
                    we.type = e.value("type", "");
                    we.weight = e.value("weight", 1.0f);
                    if (!we.type.empty()) {
                        wave.pool.push_back(we);
                    }
                }
            }
            if (!wave.pool.empty()) {
                m_Waves.push_back(wave);
            }
        }
    }

    // Load boss spawns
    if (data.contains("bosses") && data["bosses"].is_array()) {
        for (auto &entry : data["bosses"]) {
            BossEntry boss;
            boss.timeMin = entry.value("time_min", 0.0f);
            boss.type = entry.value("type", "");
            boss.evoChance = entry.value("chest_evo_chance", 0.0f);
            if (!boss.type.empty()) {
                m_Bosses.push_back(boss);
            }
        }
    }

    // Load swarm events
    if (data.contains("swarms") && data["swarms"].is_array()) {
        for (auto &entry : data["swarms"]) {
            SwarmEntry swarm;
            swarm.timeMin = entry.value("time_min", 0.0f);
            swarm.type = entry.value("type", "");
            swarm.amount = entry.value("amount", 20);
            swarm.intervalMs = entry.value("interval_ms", 100.0f);
            swarm.durationMs = entry.value("duration_ms", 5000.0f);
            if (!swarm.type.empty()) {
                m_Swarms.push_back(swarm);
            }
        }
    }

    LOG_DEBUG("Loaded wave data: {} enemy waves, {} bosses, {} swarms",
              m_Waves.size(), m_Bosses.size(), m_Swarms.size());
}

std::vector<SpawnRequest> WaveManager::Update(float dt, float gameTime,
                                               int playerLevel) {
    std::vector<SpawnRequest> requests;
    float gameMinutes = gameTime / 60.0f;

    // ── Enemy track (independent timer) ──
    int waveIdx = FindWaveForTime(gameMinutes);
    if (waveIdx >= 0 && waveIdx < static_cast<int>(m_Waves.size())) {
        auto &wave = m_Waves[waveIdx];
        m_EnemySpawnTimer += dt * 1000.0f; // convert to ms
        if (m_EnemySpawnTimer >= wave.intervalMs) {
            m_EnemySpawnTimer = 0;
            for (int i = 0; i < wave.minCount; i++) {
                std::string type = PickWeightedEnemy(wave);
                if (!type.empty()) {
                    requests.push_back({type, false, false, 1});
                }
            }
        }
    }

    // ── Boss track (independent, one-shot per entry) ──
    for (auto &boss : m_Bosses) {
        if (!boss.spawned && gameMinutes >= boss.timeMin) {
            boss.spawned = true;
            requests.push_back({boss.type, true, false, 1, boss.evoChance});
        }
    }

    // ── Swarm track (independent, triggers active swarm) ──
    for (auto &swarm : m_Swarms) {
        if (!swarm.triggered && gameMinutes >= swarm.timeMin) {
            swarm.triggered = true;
            m_ActiveSwarms.push_back(
                {swarm.type, swarm.amount, swarm.intervalMs, 0});
        }
    }

    // Process active swarms
    for (auto it = m_ActiveSwarms.begin(); it != m_ActiveSwarms.end();) {
        it->timer += dt * 1000.0f;
        if (it->timer >= it->intervalMs) {
            it->timer = 0;
            requests.push_back({it->type, false, true, 1});
            it->remaining--;
        }
        if (it->remaining <= 0) {
            it = m_ActiveSwarms.erase(it);
        } else {
            ++it;
        }
    }

    return requests;
}

std::string WaveManager::PickWeightedEnemy(const WaveEntry &wave) const {
    if (wave.pool.empty())
        return "";

    float totalWeight = 0;
    for (auto &e : wave.pool) {
        totalWeight += e.weight;
    }

    std::uniform_real_distribution<float> dist(0, totalWeight);
    float roll = dist(RNG());

    float cumulative = 0;
    for (auto &e : wave.pool) {
        cumulative += e.weight;
        if (roll <= cumulative) {
            return e.type;
        }
    }
    return wave.pool.back().type;
}

int WaveManager::FindWaveForTime(float gameTimeMinutes) const {
    int best = -1;
    for (int i = 0; i < static_cast<int>(m_Waves.size()); i++) {
        if (m_Waves[i].timeMin <= gameTimeMinutes) {
            best = i;
        }
    }
    return best;
}

} // namespace Game
