#include "Game/Resource.hpp"
#include "Game/Config.hpp"
#include "Util/Image.hpp"
#include "json.hpp"
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <memory>

namespace Game {
std::unordered_map<std::string, std::shared_ptr<Character>>
    Resource::s_Character;
std::unordered_map<std::string, std::shared_ptr<Weapon::Weapon>>
    Resource::s_Weapon;
std::unordered_map<std::string, std::shared_ptr<Projectile::Projectile>>
    Resource::s_Projectile;
std::unordered_map<std::string, Util::ObjectPool<Projectile::Projectile>>
    Resource::s_ProjectilePool;
std::unordered_map<std::string, std::shared_ptr<Util::Animation>>
    Resource::s_Animation;
std::unordered_map<std::string, std::shared_ptr<Enemy::Enemy>>
    Resource::s_Enemy;
std::unordered_map<std::string, Util::ObjectPool<Enemy::Enemy>>
    Resource::s_EnemyPool;
std::unordered_map<std::string, std::shared_ptr<Game::Passive::Passive>>
    Resource::s_Passive;

void Resource::Initialize() {
    using json = nlohmann::json;
    // TODO: load all resources
    // Character
    std::ifstream chrFile("../resources/TextAsset/v1.3.100_CHARACTER_DATA.txt");
    auto chrJson = json::parse(chrFile);
    for (auto &item : chrJson.items()) {
        s_Character[item.key()] = std::make_shared<Character>();
        auto &chr = s_Character[item.key()];
        auto data = item.value();
        auto &stats = data[0];
        std::unordered_map<std::string, float_t> stat;
        stat["amount"] = stats["amount"];
        stat["area"] = stats["area"];
        stat["armor"] = stats["armor"];
        stat["banish"] = stats["banish"];
        stat["cooldown"] = stats["cooldown"];
        stat["curse"] = stats["curse"];
        stat["duration"] = stats["duration"];
        stat["greed"] = stats["greed"];
        stat["growth"] = stats["growth"];
        stat["level"] = stats["level"];
        stat["luck"] = stats["luck"];
        stat["magnet"] = stats["magnet"];
        stat["maxHp"] = stats["maxHp"];
        stat["moveSpeed"] = stats["moveSpeed"];
        stat["power"] = stats["power"];
        stat["price"] = stats["price"];
        stat["regen"] = stats["regen"];
        stat["rerolls"] = stats["rerolls"];
        if (stats.find("revivals") != stats.end()) {
            stat["revivals"] = stats["revivals"];
        } else {
            stat["revivals"] = 0;
        }
        stat["skips"] = stats["skips"];
        stat["speed"] = stats["speed"];
        auto bgm = NULL_STRING;
        if (stats.find("bgm") != stats.end()) {
            bgm = stats["bgm"];
        }
        auto weapon = NULL_STRING;
        if (stats.find("startingWeapon") != stats.end()) {
            weapon = stats["startingWeapon"];
        }
        chr->SetBaseStats(stat);
        chr->SetInfos(item.key(), stats["charName"], stats["description"], bgm,
                      weapon);
        chr->SetDrawable(std::make_unique<::Util::Image>(
            SPRITE_PATH + stats["spriteName"].template get<std::string>()));
        if (stats.find("skins") != stats.end()) {
            for (auto &skin : stats["skins"]) {
                std::string base =
                    skin["spriteName"].template get<std::string>();
                // remove all characters after _ symbol
                base = base.substr(0, base.find('.') - 2);
                std::vector<std::shared_ptr<::Util::Image>> images;
                int32_t frames = skin["walkingFrames"];
                for (int32_t i = 1; i <= frames; i++) {
                    std::string frame = std::to_string(i);
                    if (i < 10) {
                        frame = "0" + frame;
                    }
                    images.push_back(std::make_shared<::Util::Image>(
                        SPRITE_PATH + base + frame + ".png"));
                }
                auto animation =
                    std::make_unique<Util::Animation>(images, true);
                auto animationName = static_cast<std::string>(item.key()) +
                                     skin["name"].template get<std::string>();
                s_Animation[animationName] = std::move(animation);
                chr->Load(skin["name"], s_Animation[animationName]);
            }
        } else {
            std::vector<std::shared_ptr<::Util::Image>> images;
            int32_t frames = stats["walkingFrames"];
            std::string base = stats["spriteName"].template get<std::string>();
            base = base.substr(0, base.find('.') - 2);
            for (int32_t i = 1; i <= frames; i++) {
                std::string frame = std::to_string(i);
                if (i < 10) {
                    frame = "0" + frame;
                }
                images.push_back(std::make_shared<::Util::Image>(
                    SPRITE_PATH + base + frame + ".png"));
            }
            auto animation = std::make_unique<Util::Animation>(images, true);
            auto animationName =
                static_cast<std::string>(item.key()) + "Default";
            s_Animation[animationName] = std::move(animation);
            chr->Load(item.key(), s_Animation[animationName]);
        }
    }

    // Weapon and Passive
    std::ifstream wpnFile("../resources/TextAsset/v1.3.100_WEAPON_DATA.txt");
    auto wpnJson = json::parse(wpnFile);
    for (auto &item : wpnJson.items()) {
        auto data = item.value();
        if (data[0].find("isPowerUp") != data[0].end()) {
            s_Passive[item.key()] = std::make_shared<Passive::Passive>();
            auto &passive = s_Passive[item.key()];
            auto &stats = data[0];
            std::unordered_map<std::string, float_t> stat;
            stat["level"] = stats["level"];
            stat["rarity"] = stats["rarity"];
            stat["maxLevel"] = data.size();
            std::vector<std::pair<std::string, float_t>> levelUpStat;
            for (auto &level : data) {
                for (auto &info : level.items()) {
                    if (Game::Passive::Passive::IsEffect(info.key())) {
                        levelUpStat.push_back({info.key(), info.value()});
                    }
                }
            }
            passive->SetUp(item.key(), stats["description"], stat, levelUpStat);
            passive->SetDrawable(std::make_unique<::Util::Image>(
                SPRITE_PATH + stats["frameName"].template get<std::string>()));
        } else {
            // s_Weapon[item.key()] = std::make_shared<Weapon::Weapon>();
            // auto &weapon = s_Weapon[item.key()];
            // auto &stats = data[0];
            // std::unordered_map<std::string, float_t> stat;
            // stat["amount"] = stats["amount"];
            // stat["area"] = stats["area"];
            // stat["armor"] = stats["armor"];
            // stat["banish"] = stats["banish"];
            // stat["cooldown"] = stats["cooldown"];
            // stat["curse"] = stats["curse"];
            // stat["duration"] = stats["duration"];
            // stat["greed"] = stats["greed"];
            // stat["growth"] = stats["growth"];
            // stat["level"]
        }
    }
}

std::shared_ptr<Character> Resource::GetCharacter(std::string name) {
    if (s_Character.find(name) == s_Character.end()) {
        throw std::logic_error("Character not found");
    }
    return s_Character[name];
}

std::shared_ptr<Weapon::Weapon> Resource::GetWeapon(std::string type) {
    if (s_Weapon.find(type) == s_Weapon.end()) {
        throw std::logic_error("Weapon not found");
    }
    return s_Weapon[type];
}

std::shared_ptr<Passive::Passive> Resource::GetPassive(std::string type) {
    if (s_Passive.find(type) == s_Passive.end()) {
        throw std::logic_error("Passive not found");
    }
    return s_Passive[type];
}

std::shared_ptr<Projectile::Projectile>
Resource::GetProjectile(std::string type) {
    if (s_Projectile.find(type) == s_Projectile.end()) {
        throw std::logic_error("Projectile not found");
    }
    return s_ProjectilePool[type].GetObject();
}

void Resource::ReturnProjectile(
    std::string type, std::shared_ptr<Projectile::Projectile> projectile) {
    s_ProjectilePool[type].ReturnObject(projectile);
}

std::shared_ptr<Util::Animation> Resource::GetAnimation(std::string name) {
    if (s_Animation.find(name) == s_Animation.end()) {
        throw std::logic_error("Animation not found");
    }
    return s_Animation[name];
}

std::shared_ptr<Enemy::Enemy> Resource::GetEnemy(std::string name) {
    if (s_Enemy.find(name) == s_Enemy.end()) {
        throw std::logic_error("Enemy not found");
    }
    return s_EnemyPool[name].GetObject();
}

void Resource::ReturnEnemy(std::string name,
                           std::shared_ptr<Enemy::Enemy> enemy) {
    s_EnemyPool[name].ReturnObject(enemy);
}

} // namespace Game
