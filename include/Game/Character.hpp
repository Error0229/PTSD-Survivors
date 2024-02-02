#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "Game/Util/Animated.hpp"
#include "Game/Util/Physical.hpp"
#include "pch.hpp"

#include "Util/GameObject.hpp"
#include <cmath>

namespace Game {
class Character : public ::Util::GameObject,
                  public Util::Physical,
                  public Util::Animated {
public:
    Character() = default;
    ~Character() override = default;
    void Start() override;
    void
    Update(const ::Util::Transform &transform = ::Util::Transform()) override;
    void Draw() override;

    void SetSpeed(float speed);
    void GoTo(glm::vec2 target);

    float Width() override;
    float Height() override;

    void SetBaseStats(std::unordered_map<std::string, float_t> &stats);
    void RecalculateStats();
    void SetInfos(std::string ID, std::string name, std::string description,
                  std::string bgm, std::string weapon);

private:
    std::string m_ID, m_ChrName, m_Description, m_BGM, m_Weapon;
    std::unordered_map<std::string, float_t> m_BaseStats;
    std::unordered_map<std::string, float_t> m_Coefficients;
    std::unordered_map<std::string, float_t> m_Stats;
};
} // namespace Game

#endif /* CHARACTER_HPP */
