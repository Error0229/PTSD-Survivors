#include "Game/Resource.hpp"
#include "Game/Config.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"
#include "json.hpp"
#include <cstddef>
#include <cstdint>
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
std::unordered_map<std::string, std::shared_ptr<Game::Enemy::Enemy>>
    Resource::s_Enemy;
std::unordered_map<std::string, Util::ObjectPool<Game::Enemy::Enemy>>
    Resource::s_EnemyPool;
std::unordered_map<std::string, std::shared_ptr<Game::Passive::Passive>>
    Resource::s_Passive;
std::vector<std::string> Resource::s_NormalEnemies;
std::vector<std::string> Resource::s_BossEnemies;
std::vector<std::string> Resource::s_SwarmEnemies;
std::vector<std::string> Resource::s_Characters;
std::vector<std::string> Resource::s_Weapons;
std::vector<std::string> Resource::s_Passives;
std::vector<std::string> Resource::s_Projectiles;
void Resource::Initialize() {
    using json = nlohmann::json;
    // TODO: load all resources
    // Character
    std::ifstream chrFile("../resources/TextAsset/v1.3.100_CHARACTER_DATA.txt");
    auto chrJson = json::parse(chrFile);
    for (auto &item : chrJson.items()) {
        s_Character[item.key()] = std::make_shared<Character>();
        s_Characters.push_back(item.key());
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
        chr->SetInfos(item.key(), stats["charName"].template get<std::string>(),
                      stats["description"].template get<std::string>(), bgm,
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
        // Passive
        if (data[0].find("isPowerUp") != data[0].end()) {
            s_Passive[item.key()] = std::make_shared<Passive::Passive>();
            s_Passives.push_back(item.key());
            auto &passive = s_Passive[item.key()];
            auto &stats = data[0];
            std::unordered_map<std::string, float_t> stat;
            stat["level"] = stats["level"];
            stat["rarity"] = stats["rarity"];
            stat["maxLevel"] = data.size();
            std::vector<std::vector<std::pair<std::string, float_t>>>
                levelUpStat;
            for (auto &level : data) {
                std::vector<std::pair<std::string, float_t>> temp;
                for (auto &info : level.items()) {
                    if (Game::Passive::Passive::IsEffect(info.key())) {
                        temp.push_back({info.key(), info.value()});
                    }
                }
                levelUpStat.push_back(temp);
            }
            passive->SetUp(item.key(),
                           stats["description"].template get<std::string>(),
                           stat, levelUpStat);
            passive->SetDrawable(std::make_unique<::Util::Image>(
                SPRITE_PATH + stats["frameName"].template get<std::string>()));
            // Weapon
        } else {
            s_Weapon[item.key()] = std::make_shared<Weapon::Weapon>();
            s_Weapons.push_back(item.key());
            auto &weapon = s_Weapon[item.key()];
            auto &stats = data[0];
            std::unordered_map<std::string, float_t> stat;
            stat["maxLevel"] = data.size();
            stat["rarity"] = stats["rarity"];
            stat["poolLimit"] = stats["poolLimit"];
            stat["repeatInterval"] =
                stats["repeatInterval"]; // a.k.a delay between amount of shots
            stat["hitsWalls"] = stats.value("hitsWalls", false) ? 1.0f : 0.0f;
            stat["critChance"] = stats.value("critChance", 0.0f);
            stat["critMul"] = stats.value("critMul", 1.0f);
            stat["hitVFX"] = stats.value("hitVFX", -1.0f);
            stat["knockBack"] = stats.value("knockback", 0.0f);
            stat["volume"] = stats.value("volume", 1.0f);
            stat["hitBoxDelay"] = stats.value("hitBoxDelay", 0.0f);
            stat["interval"] = stats.value("interval", 0.0f);
            stat["power"] = stats.value("power", 0.0f);
            stat["area"] = stats.value("area", 0.0f);
            stat["speed"] = stats.value("speed", 0.0f);
            stat["amount"] = stats.value("amount", 0.0f);
            stat["duration"] = stats.value("duration", 0.0f);
            stat["penetrating"] = stats.value("penetrating", 0.0f);
            std::vector<std::string> evoRequired;
            std::vector<std::string> evoFrom;
            if (stats.find("evolveFrom") != stats.end()) {
                for (auto &evo : stats["evolveFrom"]) {
                    evoFrom.push_back(evo.template get<std::string>());
                }
            }
            if (stats.find("requires") != stats.end()) {
                for (auto &evo : stats["requires"]) {
                    evoRequired.push_back(evo.template get<std::string>());
                }
            }
            std::vector<std::vector<std::pair<std::string, float_t>>>
                levelUpStat;
            for (auto &level : data) {
                std::vector<std::pair<std::string, float_t>> temp;
                for (auto &info : level.items()) {
                    if (Game::Passive::Passive::IsEffect(info.key())) {
                        temp.push_back({info.key(), info.value()});
                    }
                }
                levelUpStat.push_back(temp);
            }
            weapon->SetUp(item.key(),
                          stats["description"].template get<std::string>(),
                          evoRequired, evoFrom, stat, levelUpStat);
            weapon->SetDrawable(std::make_unique<::Util::Image>(
                SPRITE_PATH + stats["frameName"].template get<std::string>()));
            s_ProjectilePool[item.key()] =
                Util::ObjectPool<Projectile::Projectile>();
            for (int32_t i = 0; i < static_cast<int32_t>(stat["poolLimit"]);
                 i++) {
                s_ProjectilePool[item.key()].AddObject(
                    std::make_shared<Projectile::Projectile>(item.key()));
            }
        }
    }
    // Enemy
    std::ifstream enemyFile("../resources/TextAsset/v1.3.100_ENEMY_DATA.txt");
    auto enemyJson = json::parse(enemyFile);
    for (auto &item : enemyJson.items()) {
        s_Enemy[item.key()] = std::make_shared<Game::Enemy::Enemy>();
        std::unordered_map<std::string, float_t> stat;
        stat["isBoss"] = 0.0f;
        stat["isSwarm"] = 0.0f;
        if (item.key().find("BOSS") != std::string::npos) {
            stat["isBoss"] = 1.0f;
            s_BossEnemies.push_back(item.key());
        } else if (item.key().find("SWARM") != std::string::npos) {
            stat["isSwarm"] = 1.0f;
            s_SwarmEnemies.push_back(item.key());
        } else {
            s_NormalEnemies.push_back(item.key());
        }
        auto &enemy = s_Enemy[item.key()];
        auto data = item.value();
        auto &stats = data[0];
        stat["level"] = stats.value("level", 1.0f);
        stat["maxHp"] = stats.value("maxHp", 1.0f);
        stat["speed"] = stats.value("speed", 1.0f);
        stat["power"] = stats.value("power", 1.0f);
        stat["knockBack"] = stats.value("knockback", 1.0f);
        stat["xp"] = stats.value("xp", 1.0f);
        stat["scale"] = stats.value("scale", 0.0f);
        stat["end"] = stats.value("end", 0.0f);
        stat["tint"] = stats.value("tint", 0.0f);
        stat["passThroughWalls"] =
            stats.value("passThroughWalls", false) ? 1.0f : 0.0f;

        enemy->SetUp(item.key(), stat);
        std::vector<std::shared_ptr<::Util::Image>> images;
        int32_t frames = 0;
        if (stats.find("idleFrameCount") != stats.end()) {
            frames = stats["idleFrameCount"];
        } else {
            images.push_back(std::make_shared<::Util::Image>(
                SPRITE_PATH +
                stats["frameNames"][0].template get<std::string>()));
            LOG_DEBUG("No idleFrame found for enemy: " + item.key());
        }
        std::string base = stats["frameNames"][0].template get<std::string>();
        base = base.substr(0, base.find('.') - 1);
        for (int32_t i = 1; i <= frames; i++) {
            std::string frame = std::to_string(i);
            if (i < 10) {
                frame = "0" + frame;
            }
            images.push_back(std::make_shared<::Util::Image>(
                SPRITE_PATH + base + "i" + frame + ".png"));
        }
        auto animation = std::make_unique<Util::Animation>(images, true);
        auto animationName = static_cast<std::string>(item.key()) + "Idle";
        s_Animation[animationName] = std::move(animation);
        enemy->Load("Idle", s_Animation[animationName]);

        images.clear();
        for (int32_t i = 0; i < stats["end"]; i++) {
            images.push_back(std::make_shared<::Util::Image>(
                SPRITE_PATH + base + std::to_string(i) + ".png"));
        }
        animation = std::make_unique<Util::Animation>(images, false);
        animationName = static_cast<std::string>(item.key()) + "Death";
        s_Animation[animationName] = std::move(animation);
        enemy->Load("Death", s_Animation[animationName]);
        enemy->SetAnimation("Idle");
        s_EnemyPool[item.key()] = Util::ObjectPool<Game::Enemy::Enemy>();
        for (int32_t i = 0; i < 10; i++) {
            s_EnemyPool[item.key()].AddObject(enemy);
        }
    }
    // hitVFX
    std::ifstream hitVFXFile("../resources/TextAsset/v1.3.100_HITVFX_DATA.txt");
    auto hitVFXJson = json::parse(hitVFXFile);
    for (auto &item : hitVFXJson.items()) {
        if (item.key() == "0") {
            continue;
        }
        auto data = item.value();
        auto images = std::vector<std::shared_ptr<::Util::Image>>();
        images.emplace_back(std::make_shared<::Util::Image>(
            SPRITE_PATH + data["hitFrameName"].template get<std::string>()));
        images.emplace_back(std::make_shared<::Util::Image>(
            SPRITE_PATH + data["impactFrameName"].template get<std::string>()));
        auto animation = std::make_unique<Util::Animation>(
            images, false, data["duration"].template get<int32_t>());
        auto animationName = "VFX" + item.key();
        s_Animation[animationName] = std::move(animation);
    }
    // projectiles
    // WHIP
    std::vector<std::string> paths = {
        SPRITE_PATH + "slash.png",
        SPRITE_PATH + "none.png",
    };
    auto animation = std::make_shared<Game::Util::Animation>(paths, true, 200);
    s_Animation["WHIP"] = animation;
    paths.clear();
}
bool Resource::HaveAnimation(std::string name) {
    return s_Animation.find(name.data()) != s_Animation.end();
}

std::shared_ptr<Character> Resource::GetCharacter(std::string name) {
    if (s_Character.find(name) == s_Character.end()) {
        throw std::logic_error("Character not found");
    }
    return s_Character[name];
}

std::shared_ptr<Weapon::Weapon> Resource::GetWeapon(std::string type) {
    if (s_Weapon.find(type) == s_Weapon.end()) {
        for (auto &content : s_Weapon) {
            LOG_DEBUG("Key: {}", content.first);
            LOG_DEBUG("Value: {}", content.second->ID());
        }
        LOG_ERROR("Weapon {} Not Found", type.data());
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
