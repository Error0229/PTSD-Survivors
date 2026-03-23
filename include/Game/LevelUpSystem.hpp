#ifndef LEVEL_UP_SYSTEM_HPP
#define LEVEL_UP_SYSTEM_HPP

#include "Game/Passive/Passive.hpp"
#include "Game/Weapon/Weapon.hpp"
#include "pch.hpp"
#include <set>
#include <string>
#include <vector>

namespace Game {

struct LevelUpChoice {
    std::string id;
    enum class Category {
        NEW_WEAPON,
        UPGRADE_WEAPON,
        NEW_PASSIVE,
        UPGRADE_PASSIVE,
        GOLD
    } category;
    std::string description;
    int currentLevel = 0;
    int maxLevel = 0;
};

class LevelUpSystem {
public:
    static std::vector<LevelUpChoice>
    GenerateChoices(int count, const std::set<std::string> &ownedItems,
                    const std::set<std::shared_ptr<Weapon::Weapon>> &weapons,
                    const std::set<std::shared_ptr<Passive::Passive>> &passives,
                    const std::vector<std::string> &allWeaponIDs,
                    const std::vector<std::string> &allPassiveIDs);

    static constexpr int WEAPON_CAP = 6;
    static constexpr int PASSIVE_CAP = 6;
};

} // namespace Game

#endif // LEVEL_UP_SYSTEM_HPP
