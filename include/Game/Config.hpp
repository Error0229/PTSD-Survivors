#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "pch.hpp"
namespace Game {
constexpr const char *MAP_SPRITE_PATH = "../assets/sprites/map/";
constexpr const char *CHARACTER_SPRITE_PATH = "../assets/sprites/character/";
const std::string SPRITE_PATH = "../resources/Sprite/";
constexpr const char *SPRITE_EXT = ".png";
constexpr int32_t MAP_PADDING_NUM = 4;
constexpr int32_t DEFAULT_ANIMATION_FRAME_TIME = 100;
constexpr float MAP_LAYER = 1;
constexpr float ENEMY_LAYER = 2;
constexpr float CHARACTER_LAYER = 3;
constexpr float PROJECTILE_LAYER = 4;
const std::string ENEMY = "Enemy";
const std::string CHARACTER = "Character";
const std::string PROJECTILE = "Projectile";
// sha256 string won't collide, right?
const std::string NULL_STRING =
    "16e81418a36c58995e00aeb9c261c9ef7fb739b37c72351f511dc3b3a89e5962";
} // namespace Game

#endif /* CONFIG_HPP */
