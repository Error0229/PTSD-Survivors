#include "Game/LevelUpSystem.hpp"
#include "Game/Passive/Passive.hpp"
#include "Game/Resource.hpp"
#include "Game/Weapon/Weapon.hpp"
#include <algorithm>
#include <random>

namespace Game {

static std::mt19937 &RNG() {
    static std::mt19937 gen(std::random_device{}());
    return gen;
}

std::vector<LevelUpChoice> LevelUpSystem::GenerateChoices(
    int count,
    const std::set<std::string> &ownedItems,
    const std::set<std::shared_ptr<Weapon::Weapon>> &weapons,
    const std::set<std::shared_ptr<Passive::Passive>> &passives,
    const std::vector<std::string> &allWeaponIDs,
    const std::vector<std::string> &allPassiveIDs) {

    std::vector<LevelUpChoice> pool;

    // New weapons (not owned, under weapon cap, exclude evolutions)
    if (static_cast<int>(weapons.size()) < WEAPON_CAP) {
        for (auto &id : allWeaponIDs) {
            if (ownedItems.count(id) == 0) {
                // Skip evolution-only weapons — they're obtained via chests
                try {
                    auto def = Resource::GetWeapon(id);
                    if (!def->GetEvoFrom().empty())
                        continue; // this is an evolved weapon
                } catch (...) {
                    continue;
                }
                pool.push_back(
                    {id, LevelUpChoice::Category::NEW_WEAPON, "New: " + id, 0, 0});
            }
        }
    }

    // Weapon upgrades (owned, not max level)
    for (auto &weapon : weapons) {
        if (!weapon->IsMaxLevel()) {
            pool.push_back({weapon->ID(),
                            LevelUpChoice::Category::UPGRADE_WEAPON,
                            "Upgrade: " + weapon->ID(),
                            weapon->GetLevel(),
                            weapon->GetMaxLevel()});
        }
    }

    // New passives (not owned, under passive cap)
    if (static_cast<int>(passives.size()) < PASSIVE_CAP) {
        for (auto &id : allPassiveIDs) {
            if (ownedItems.count(id) == 0) {
                pool.push_back(
                    {id, LevelUpChoice::Category::NEW_PASSIVE, "New: " + id, 0, 0});
            }
        }
    }

    // Passive upgrades (owned, not max level)
    for (auto &passive : passives) {
        if (passive->GetLevel() < passive->GetMaxLevel()) {
            pool.push_back({passive->ID(),
                            LevelUpChoice::Category::UPGRADE_PASSIVE,
                            "Upgrade: " + passive->ID(),
                            passive->GetLevel(),
                            passive->GetMaxLevel()});
        }
    }

    // TODO-004: Fallback if pool is empty or too small
    while (static_cast<int>(pool.size()) < count) {
        pool.push_back({"GOLD", LevelUpChoice::Category::GOLD, "+25 Gold", 0, 0});
    }

    // Weighted random sample without duplicates
    std::shuffle(pool.begin(), pool.end(), RNG());
    if (static_cast<int>(pool.size()) > count) {
        pool.resize(count);
    }

    return pool;
}

} // namespace Game
