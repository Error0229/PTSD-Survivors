#include "Game/Manager.hpp"
#include "Game/Camera.hpp"
#include "Game/Config.hpp"
#include "Game/Projectile/Projectile.hpp"
#include "Game/Resource.hpp"
#include "Game/Util/Physical.hpp"
#include "Game/Util/Timer.hpp"
#include "Util/Color.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/Transform.hpp"
#include "Util/TransformUtils.hpp"
#include "config.hpp"
#include "pch.hpp"
#include <memory>
#include <random>
namespace Game {
Manager CAT;
void Manager::Start() {
    Resource::Initialize();
    ::Util::Image::BuildAtlases();
    m_Character = Resource::GetCharacter("TATANKA");
    m_Character->SetAnimation("Default");
    m_Character->Start();
    m_Map = std::make_shared<Map>();
    m_Map->Setup("dummy5");
    m_Map->Start();
    const std::string fontPath = RESOURCE_BASE + "Font/ANY.ttf";
    m_FPS = std::make_shared<::Util::Text>(
        fontPath, 24, "FPS: 0",
        ::Util::Color{135 / 255., 206 / 255., 235 / 255., 1});
    m_ChrPos = std::make_shared<::Util::Text>(
        fontPath, 24, "ChrPos: 0",
        ::Util::Color{135 / 255., 206 / 255., 235 / 255., 1});
    m_Plain = std::make_shared<Util::QuadTree>(
        0, 0, PTSD_Config::WINDOW_WIDTH, PTSD_Config::WINDOW_HEIGHT,
        QUADTREE_MAX_OBJECTS, QUADTREE_MAX_LEVELS, 0);
    AcquireWeapon("VAMPIRICA");
    // Load wave data if available
    m_WaveManager.Load(RESOURCE_BASE + "Data/waves/stage1.json");
    Util::Clock.Start();
}
void Manager::Update() {
    if (m_State != GameState::PLAYING) {
        return; // Only update game systems while playing
    }
    m_GameTime += ::Util::Time::GetDeltaTime();
    m_Character->Update({::Util::Input::GetCursorPosition()});
    m_Map->Update({m_Character->GetPosition()});

    // Wave-based enemy spawning
    float dt = ::Util::Time::GetDeltaTime();
    auto spawnRequests =
        m_WaveManager.Update(dt, m_GameTime, m_Character->GetLevel());
    for (auto &req : spawnRequests) {
        try {
            auto enemy = Resource::GetEnemy(req.enemyType);
            enemy->Start();
            // Spawn at random position around viewport edge
            float halfW = PTSD_Config::WINDOW_WIDTH * 0.5f + 50.0f;
            float halfH = PTSD_Config::WINDOW_HEIGHT * 0.5f + 50.0f;
            auto chrPos = m_Character->GetPosition();
            std::uniform_int_distribution<int> edgeDist(0, 3);
            std::uniform_real_distribution<float> posDist(-1.0f, 1.0f);
            static std::mt19937 rng(std::random_device{}());
            int edge = edgeDist(rng);
            glm::vec2 spawnPos = chrPos;
            switch (edge) {
            case 0: spawnPos += glm::vec2{posDist(rng) * halfW, -halfH}; break;
            case 1: spawnPos += glm::vec2{posDist(rng) * halfW, halfH}; break;
            case 2: spawnPos += glm::vec2{-halfW, posDist(rng) * halfH}; break;
            case 3: spawnPos += glm::vec2{halfW, posDist(rng) * halfH}; break;
            }
            enemy->SetPosition(spawnPos);
            if (req.isBoss) {
                enemy->SetScale(m_Character->GetLevel());
                enemy->Set("hp", enemy->Get("maxHp"));
                enemy->Set("chestEvoChance", req.chestEvoChance);
            }
            m_Enemies.insert(enemy);
        } catch (const std::exception &e) {
            LOG_WARN("Failed to spawn {}: {}", req.enemyType, e.what());
        }
    }

    // Special timed pickups
    int currentMinute = static_cast<int>(m_GameTime / 60.0f);
    if (currentMinute > 0 && currentMinute % 5 == 0 &&
        currentMinute != m_LastVacuumMinute) {
        m_LastVacuumMinute = currentMinute;
        Pickup vacuum;
        vacuum.Spawn(Pickup::Type::VACUUM, m_Character->GetPosition() +
                     glm::vec2{100, 0});
        m_Pickups.push_back(std::move(vacuum));
    }

    std::for_each(m_Weapons.begin(), m_Weapons.end(),
                  [](auto &weapon) { weapon->Update(); });
    std::erase_if(m_Projectiles, [](auto &projectile) {
        projectile->Update();
        if (projectile->IsOver()) {
            // Return to pool for reuse
            Resource::ReturnProjectile(
                std::string(projectile->ID()), projectile);
            return true;
        }
        return false;
    });
    std::vector<std::shared_ptr<Enemy::Enemy>> toErase;
    for (auto &enemy : m_Enemies) {
        enemy->Update({m_Character->GetPosition()});
        if (enemy->IsOver()) {
            // Spawn pickups on death
            if (enemy->IsDead() && m_Pickups.size() < 500) {
                // XP gem
                Pickup xpPickup;
                float xp = enemy->Get("xp");
                Pickup::Type xpType = Pickup::Type::XP_SMALL;
                if (xp >= 10)
                    xpType = Pickup::Type::XP_LARGE;
                else if (xp >= 3)
                    xpType = Pickup::Type::XP_MED;
                xpPickup.Spawn(xpType, enemy->GetPosition());
                m_Pickups.push_back(std::move(xpPickup));

                // Boss enemies drop chests with evo chance from wave data
                if (enemy->IsBoss()) {
                    Pickup chest;
                    chest.Spawn(Pickup::Type::CHEST, enemy->GetPosition());
                    chest.SetEvoChance(enemy->Get("chestEvoChance"));
                    m_Pickups.push_back(std::move(chest));
                }
            }
            toErase.push_back(enemy);
        }
    }
    for (auto &enemy : toErase) {
        Resource::ReturnEnemy(enemy->ID(), enemy);
        m_Enemies.erase(enemy);
    }
    // handle projectile collision
    m_Plain->Clear();
    m_Plain->SetRange(m_Character->GetPosition().x - 1000,
                      m_Character->GetPosition().y - 1000, 2000, 2000);
    for (auto &projectile : m_Projectiles) {
        m_Plain->Insert(projectile);
    }
    for (auto &enemy : m_Enemies) {
        m_Plain->Insert(enemy);
    }
    std::vector<std::shared_ptr<Util::Physical>> result;
    for (auto &enemy : m_Enemies) {
        m_Plain->QueryCollision(enemy, PROJECTILE, result);
        for (auto &other : result) {
            if (enemy->IsCollideWith(other)) {
                enemy->CollisionWith(
                    std::static_pointer_cast<Projectile::Projectile>(other));
            }
        }
        result.clear();
    }
    // handle enemy collision
    result.clear();
    for (auto &enemy : m_Enemies) {
        m_Plain->QueryCollision(enemy, ENEMY, result);
        for (auto &other : result) {
            enemy->CollisionWith(std::static_pointer_cast<Enemy::Enemy>(other));
        }
        if (enemy->IsCollideWith(
                std::static_pointer_cast<Game::Util::Physical>(m_Character))) {
            enemy->CollisionWith(m_Character);
        }
        result.clear();
    }
    toErase.clear();

    // ── Pickup update (flat distance check, not in QuadTree) ──
    float magnetRange = 30.0f + Passive::Passive::GetEffect("magnet");
    auto chrPos = m_Character->GetPosition();
    for (auto it = m_Pickups.begin(); it != m_Pickups.end();) {
        it->Update(dt, chrPos, magnetRange);
        if (it->IsCollected()) {
            // Apply pickup effect
            switch (it->GetType()) {
            case Pickup::Type::XP_SMALL:
            case Pickup::Type::XP_MED:
            case Pickup::Type::XP_LARGE: {
                float growth = 1.0f + Passive::Passive::GetEffect("growth");
                int levels = m_Character->AddXP(it->GetValue() * growth);
                m_PendingLevelUps += levels;
                break;
            }
            case Pickup::Type::CHEST: {
                // Chest opening: check evo chance before attempting evolution
                bool evolved = false;
                float evoChance = it->GetEvoChance();
                if (evoChance > 0) {
                    static std::mt19937 chestRng(std::random_device{}());
                    std::uniform_real_distribution<float> dist(0, 1);
                    if (dist(chestRng) < evoChance) {
                        evolved = TryEvolveWeapon();
                    }
                }
                if (!evolved) {
                    m_PendingLevelUps++;
                }
                break;
            }
            case Pickup::Type::CHICKEN:
                // Heal
                m_Character->AddHP(it->GetValue());
                break;
            case Pickup::Type::ROSARY:
                // Kill all on-screen enemies
                for (auto &enemy : m_Enemies) {
                    enemy->Hurt(999999);
                }
                break;
            case Pickup::Type::VACUUM:
                // Attract all pickups immediately
                for (auto &p : m_Pickups) {
                    p.Update(0, chrPos, 99999.0f);
                }
                break;
            default:
                break;
            }
            it = m_Pickups.erase(it);
        } else if (it->ShouldRemove()) {
            it = m_Pickups.erase(it);
        } else {
            ++it;
        }
    }

    // ── Check for pending level-ups ──
    if (m_PendingLevelUps > 0 && m_State == GameState::PLAYING) {
        // Generate choices
        auto choices = LevelUpSystem::GenerateChoices(
            4, m_Have, m_Weapons, m_Passives,
            Resource::s_Weapons, Resource::s_Passives);

        if (choices.empty()) {
            // TODO-004 fallback: nothing to offer, skip
            m_PendingLevelUps--;
        } else {
            // Phase 5 will show UI. For now, auto-pick the first choice.
            auto &pick = choices[0];
            switch (pick.category) {
            case LevelUpChoice::Category::NEW_WEAPON:
                AcquireWeapon(pick.id);
                break;
            case LevelUpChoice::Category::UPGRADE_WEAPON:
                for (auto &w : m_Weapons) {
                    if (w->ID() == pick.id && !w->IsMaxLevel()) {
                        w->LevelUp();
                        break;
                    }
                }
                break;
            case LevelUpChoice::Category::NEW_PASSIVE:
                AcquirePassive(pick.id);
                break;
            case LevelUpChoice::Category::UPGRADE_PASSIVE:
                for (auto &p : m_Passives) {
                    if (p->ID() == pick.id && p->GetLevel() < p->GetMaxLevel()) {
                        p->LevelUp();
                        break;
                    }
                }
                break;
            case LevelUpChoice::Category::GOLD:
                // +25 gold — no-op for now (no gold system yet)
                break;
            }
            m_PendingLevelUps--;
        }
    }

    // ── Check for game over ──
    if (m_Character->IsDead() && m_State == GameState::PLAYING) {
        m_State = GameState::GAME_OVER;
    }

    int drawCalls = m_Batch ? m_Batch->GetDrawCallCount() : 0;
    int spriteCount = m_Batch ? m_Batch->GetSpriteCount() : 0;
    float fps = ::Util::Time::GetDeltaTime() > 0
                    ? 1.0f / ::Util::Time::GetDeltaTime()
                    : 0;
    m_FPS->SetText("FPS:" + std::to_string(static_cast<int>(fps)) +
                   "  DC:" + std::to_string(drawCalls) +
                   "  Sprites:" + std::to_string(spriteCount) +
                   "  Enemies:" + std::to_string(m_Enemies.size()));
    int gameMin = static_cast<int>(m_GameTime / 60.0f);
    int gameSec = static_cast<int>(m_GameTime) % 60;
    m_ChrPos->SetText("Lv:" + std::to_string(m_Character->GetLevel()) +
                      "  XP:" + std::to_string(static_cast<int>(m_Character->GetXP())) +
                      "/" + std::to_string(static_cast<int>(m_Character->GetMaxXP())) +
                      "  Time:" + std::to_string(gameMin) + ":" +
                      (gameSec < 10 ? "0" : "") + std::to_string(gameSec) +
                      "  Pickups:" + std::to_string(m_Pickups.size()));
}
void Manager::Draw() {
    // Lazily initialize SpriteBatch
    if (!m_Batch) {
        m_Batch = std::make_unique<Core::SpriteBatch>(8192);
    }

    glm::mat4 viewProjection = ::Util::ComputeViewProjection();

    // Begin batched rendering
    ::Util::Image::SetBatch(m_Batch.get());
    m_Batch->Begin(viewProjection);

    m_Map->Draw();
    for (auto &pickup : m_Pickups) {
        pickup.Draw();
    }
    m_Character->Draw();
    std::for_each(m_Enemies.begin(), m_Enemies.end(),
                  [](auto &enemy) { enemy->Draw(); });
    for (auto &projectile : m_Projectiles) {
        projectile->Draw();
    }

    // Flush batched sprites
    m_Batch->End();
    ::Util::Image::SetBatch(nullptr);

    // Draw text overlays in immediate mode (not batched — Text uses its own
    // rendering path)
    {
        ::Util::Transform fpsTransform{{-280, 275}, 0, {1, 1}};
        auto fpsData = ::Util::ConvertToUniformBufferData(
            fpsTransform, m_FPS->GetSize(), 3);
        m_FPS->Draw(fpsData);
    }
    {
        ::Util::Transform chrPosTransform{{-280, 250}, 0, {1, 1}};
        auto chrPosData = ::Util::ConvertToUniformBufferData(
            chrPosTransform, m_ChrPos->GetSize(), 3);
        m_ChrPos->Draw(chrPosData);
    }
}
bool Manager::Have(std::string name) {
    return m_Have.count(name) > 0;
}

void Manager::EnemyGen() { /*
     static std::random_device rd;
     static std::mt19937 gen(rd());
     std::uniform_int_distribution<> distribute(0,
     Resource::s_NormalEnemies.size() - 1); auto enemyName =
     std::next(Resource::s_NormalEnemies.begin(), distribute(gen));*/
    static size_t i = 0;
    static std::string enemyName;
    if (i < Resource::s_NormalEnemies.size()) {
        enemyName = Resource::s_NormalEnemies[i++];
    } else {
        enemyName =
            Resource::s_BossEnemies[i - Resource::s_NormalEnemies.size()];
        i++;
    }
    auto enemy = Resource::GetEnemy(enemyName);
    enemy->Start();
    m_Enemies.insert(enemy);
}
void Manager::HurtEnemy() {
    for (auto &enemy : m_Enemies) {
        enemy->Hurt(100);
    }
}
void Manager::AcquireWeapon(std::string name) {
    m_Have.insert(name);
    auto weapon = Resource::GetWeapon(name);
    weapon->Start();
    m_Weapons.insert(weapon);
}
void Manager::AcquirePassive(std::string name) {
    m_Have.insert(name);
    auto passive = Resource::GetPassive(name);
    passive->Start();
    m_Passives.insert(passive);
}

void Manager::AddProjectile(
    std::shared_ptr<Projectile::Projectile> projectile) {
    m_Projectiles.insert(projectile);
}

bool Manager::TryEvolveWeapon() {
    // Check all weapon definitions for evolution candidates.
    // An evolved weapon has "evolveFrom" (base weapons needed, must be max level)
    // and "requires" (passives needed, must be owned).
    for (auto &evoID : Resource::s_Weapons) {
        auto evoDef = Resource::GetWeapon(evoID);
        auto &evoFrom = evoDef->GetEvoFrom();
        auto &evoReq = evoDef->GetEvoRequired();

        if (evoFrom.empty())
            continue; // not an evolution

        // Check: do we own all required base weapons at max level?
        bool hasAllBases = true;
        for (auto &baseID : evoFrom) {
            bool found = false;
            for (auto &owned : m_Weapons) {
                if (owned->ID() == baseID && owned->IsMaxLevel()) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                hasAllBases = false;
                break;
            }
        }
        if (!hasAllBases)
            continue;

        // Check: do we own all required passives?
        bool hasAllPassives = true;
        for (auto &reqID : evoReq) {
            if (m_Have.count(reqID) == 0) {
                hasAllPassives = false;
                break;
            }
        }
        if (!hasAllPassives)
            continue;

        // Evolution! Remove base weapon(s), add evolved weapon
        for (auto &baseID : evoFrom) {
            for (auto it = m_Weapons.begin(); it != m_Weapons.end(); ++it) {
                if ((*it)->ID() == baseID) {
                    m_Weapons.erase(it);
                    break;
                }
            }
        }

        // Clean up in-flight projectiles from old weapon (Codex issue #4)
        for (auto &baseID : evoFrom) {
            std::erase_if(m_Projectiles, [&](auto &proj) {
                return proj->ID() == baseID;
            });
        }

        AcquireWeapon(evoID);
        LOG_DEBUG("Evolved → {}", evoID);
        return true; // one evolution per chest
    }
    return false;
}

} // namespace Game
