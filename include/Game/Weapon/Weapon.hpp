#include "Game/Passive/Passive.hpp"
#include "Util/GameObject.hpp"
#include "pch.hpp"
#include <cmath>
namespace Game::Weapon {
enum class Type;
struct DynamicStat {
    float_t Damage, Area, Speed;
    int32_t Amount;
    clock_t Duration, CoolDown;
};
class Weapon : public ::Util::GameObject {
public:
    Weapon() = default;
    virtual ~Weapon() = 0;

    int32_t GetLevel();
    int32_t GetMaxLevel();
    std::string GetLevelUpMessage();

    bool IsMaxLevel();
    bool IsEvo();
    bool CanEvo();
    void RecalculateStat();
    void UpdateModifier(DynamicStat &modifier);

    virtual void Start() override;
    virtual void
    Update(const ::Util::Transform &transform = ::Util::Transform()) override;
    virtual void Draw() override;
    virtual void LevelUp() = 0;

    static void Initialize();
    static std::map<Type, std::shared_ptr<Weapon>> s_BaseWeapons;
    static std::vector<std::shared_ptr<Weapon>> s_Weapons;

protected:
    int32_t m_Level, m_MaxLevel, m_Rarity, m_Amount, m_Pierce, m_PoolLimit,
        m_BlockByWall;
    float_t m_Area, m_Damage, m_Chance, m_CritMulti, m_Speed, m_KnockBack;
    Type m_Type, m_EvoType;
    Game::Passive::PassiveType EvoRequirement;
    DynamicStat m_BaseState, m_Modifier;
    clock_t m_Duration, m_Interval, m_LastTimeAttack, m_HitboxDelay, m_CoolDown;
    std::vector<std::string> m_LevelUpMessage;
};

enum class Type {
    NONE,
    WHIP,
    MAGIC_MISSILE,
    KNIFE,
    AXE,
    CROSS,
    HOLYBOOK,
    FIREBALL,
    GARLIC,
    HOLYWATER,
    DIAMOND,
    LIGHTNING,
    PENTAGRAM,
    SILF,
    SILF2,
    GUNS,
    GUNS2,
    GATTI,
    SONG,
    TRAPING,
    LANCET,
    LAUREL,
    VENTO,
    BONE,
    CHERRY,
    CART2,
    FLOWER,
    LAROBBA,
    JUBILEE,
    TRIASSO1,
    CANDYBOX,
    VICTORY,
    MISSPELL,
    VAMPIRICA,
    HOLY_MISSILE,
    THOUSAND,
    SCYTHE,
    HEAVENSWORD,
    VESPERS,
    HELLFIRE,
    VORTEX,
    BORA,
    ROCHER,
    LOOP,
    SIRE,
    STIGRANGATTI,
    MANNAGGIA,
    TRAPANO2,
    MISSPELL2,
    CORRIDOR,
    SHROUD,
    TRIASSO2,
    TRIASSO3,
    GUNS3,
    SILF3,
    VENTO2,
    SOLES,
    CANDYBOX2
};
} // namespace Game::Weapon
