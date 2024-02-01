#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "pch.hpp"
namespace Game {
constexpr const char *MAP_SPRITE_PATH = "../assets/sprites/map/";
constexpr const char *CHARACTER_SPRITE_PATH = "../assets/sprites/character/";
constexpr const char *SPRITE_EXT = ".png";
constexpr int32_t MAP_PADDING_NUM = 4;
constexpr float MAP_LAYER = 1;
constexpr float CHARACTER_LAYER = 2;
// sha256 string won't collide, right?
const std::string NULL_STRING =
    "16e81418a36c58995e00aeb9c261c9ef7fb739b37c72351f511dc3b3a89e5962";
} // namespace Game

#endif /* CONFIG_HPP */
