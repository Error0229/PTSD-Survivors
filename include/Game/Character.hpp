#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "Game/Util/Animated.hpp"
#include "Game/Util/Mirrored.hpp"
#include "Game/Util/Physical.hpp"
#include "pch.hpp"

#include "Util/GameObject.hpp"
#include <cmath>

namespace Game {
class Character : public ::Util::GameObject,
                  public Util::Physical,
                  public Util::Animated,
                  public Util::Mirrored {
public:
    Character() = default;
    ~Character() override = default;
    void Start();
    void Update(const ::Util::Transform &transform = ::Util::Transform());
    void Draw() override;

    void SetSpeed(float speed);
    void GoTo(glm::vec2 target);

    float_t Width() const override;
    float_t Height() const override;
    float_t Rotation() const override;
    void SetBaseStats(std::unordered_map<std::string, float_t> &stats);
    void RecalculateStats();
    void SetInfos(std::string ID, std::string name, std::string description,
                  std::string bgm, std::string weapon);
    void Hurt(float_t damage);
    void AddHP(float_t amount) {
        m_["hp"] = std::min(m_["hp"] + amount, m_["maxHp"]);
    }

    // XP / Level
    int GetLevel() const { return m_Level; }
    float GetXP() const { return m_XP; }
    float GetMaxXP() const { return m_MaxXP; }
    int AddXP(float amount); // returns number of levels gained (0 if none)
    std::string GetStartingWeapon() const { return m_Weapon; }
    std::string GetID() const { return m_ID; }
    bool IsDead() const {
        auto it = m_.find("hp");
        return it != m_.end() && it->second <= 0;
    }

private:
    std::string m_ID, m_ChrName, m_Description, m_BGM, m_Weapon;
    std::unordered_map<std::string, float_t> m_BaseStats;
    std::unordered_map<std::string, float_t> m_Coefficients;
    std::unordered_map<std::string, float_t> m_;
    int m_Level = 1;
    float m_XP = 0;
    float m_MaxXP = 15.0f; // level 1 threshold

    float ComputeMaxXP(int level) const;
};
} // namespace Game

#endif /* CHARACTER_HPP */
