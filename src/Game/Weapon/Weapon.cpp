#include "Game/Weapon/Weapon.hpp"
#include "Game/Camera.hpp"
#include "Game/Manager.hpp"
#include "Game/Resource.hpp"
#include "Game/Util/Timer.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"
#include "Util/TransformUtils.hpp"
#include <string>
#include <unordered_map>

namespace Game::Weapon {

// ── Behavior Factory ────────────────────────────────────
// Maps weapon ID to the projectile behavior variant it should create.
// This is the single source of truth for weapon → behavior mapping.

Projectile::BehaviorVariant
CreateBehaviorForWeapon(const std::string &weaponID) {
    // StaticFieldAoE: follows player at fixed offset
    if (weaponID == "WHIP" || weaponID == "VAMPIRICA" ||
        weaponID == "GARLIC" || weaponID == "VORTEX") {
        return Projectile::StaticFieldAoE{};
    }
    // DirectLine: targets nearest enemy
    if (weaponID == "MAGIC_MISSILE" || weaponID == "HOLY_MISSILE") {
        return Projectile::DirectLine{};
    }
    // CursorDirection: fires toward cursor
    if (weaponID == "KNIFE" || weaponID == "THOUSAND") {
        return Projectile::CursorDirection{};
    }
    // ParabolicArc: gravity physics
    if (weaponID == "AXE" || weaponID == "SCYTHE") {
        return Projectile::ParabolicArc{};
    }
    // BoomerangArc: parabolic with return
    if (weaponID == "CROSS" || weaponID == "HEAVENSWORD") {
        return Projectile::BoomerangArc{};
    }
    // OrbitalCircle: orbits player
    if (weaponID == "HOLYBOOK") {
        return Projectile::OrbitalCircle{};
    }
    if (weaponID == "VESPERS") {
        Projectile::OrbitalCircle orbit;
        orbit.permanent = true;
        return orbit;
    }
    // ConeSpread: fires within cone
    if (weaponID == "FIREBALL" || weaponID == "HELLFIRE") {
        return Projectile::ConeSpread{};
    }
    // BouncingWall: bounces off screen edges
    if (weaponID == "DIAMOND" || weaponID == "ROCHER") {
        return Projectile::BouncingWall{};
    }
    // SeekingExplosion: homes then AoE
    if (weaponID == "HOLYWATER" || weaponID == "BORA") {
        return Projectile::SeekingExplosion{};
    }
    // Default: unimplemented weapons
    return Projectile::Unimplemented{};
}

// ── Weapon Setup & Stats ────────────────────────────────

void Weapon::SetUp(
    std::string ID, std::string Description,
    std::vector<std::string> EvoRequired, std::vector<std::string> EvoFrom,
    std::unordered_map<std::string, float_t> BaseStats,
    std::vector<std::vector<std::pair<std::string, float_t>>> LevelUpStat) {
    m_ID = ID;
    m_Description = Description;
    m_Base = BaseStats;
    m_ = BaseStats;
    m_LevelUpStat = LevelUpStat;
    m_EvoRequired = EvoRequired;
    m_EvoFrom = EvoFrom;
    m_["level"] = 0;
    m_["maxLevel"] = static_cast<float_t>(m_LevelUpStat.size());
    m_LastTimeAttack = -1;
}

std::string Weapon::ID() { return m_ID; }

int32_t Weapon::GetLevel() { return static_cast<int32_t>(m_["level"]); }
int32_t Weapon::GetMaxLevel() {
    return static_cast<int32_t>(m_["maxLevel"]);
}
std::string Weapon::GetLevelUpMessage() {
    return m_LevelUpMessage[GetLevel() - 1];
}

void Weapon::Start() {
    m_["level"] = 1;
    m_["maxLevel"] = static_cast<float_t>(m_LevelUpStat.size());
    // Initialize modifier map with multiplicative identity (1.0)
    // so RecalculateStat doesn't zero everything out
    if (m_Mod.empty()) {
        m_Mod["damage"] = 1.0f;
        m_Mod["power"] = 1.0f;
        m_Mod["area"] = 1.0f;
        m_Mod["speed"] = 1.0f;
        m_Mod["amount"] = 0.0f; // additive
        m_Mod["duration"] = 1.0f;
        m_Mod["coolDown"] = 1.0f;
    }
    RecalculateStat();
}

void Weapon::Update(const ::Util::Transform &transform) {
    (void)transform;
    // Skip firing for permanent weapons that already spawned (VESPERS)
    if (m_PermanentFired)
        return;
    // Check cooldown
    auto now = Util::Clock.Now();
    if (now - m_LastTimeAttack < static_cast<time_t>(m_["interval"])) {
        return;
    }
    m_LastTimeAttack = now;
    FireProjectiles();
}

void Weapon::FireProjectiles() {
    auto chrPos = Camera::s_Position;
    auto mouse = Camera::ScreenToWorld(::Util::Input::GetCursorPosition());
    int amount = static_cast<int>(m_["amount"]);
    if (amount <= 0)
        amount = 1;

    // Build behavior context
    Projectile::BehaviorContext ctx;
    ctx.playerPos = chrPos;
    ctx.totalAmount = amount;

    // Cursor direction
    glm::vec2 cursorDiff = mouse - chrPos;
    float cursorLen = glm::length(cursorDiff);
    ctx.cursorDir = (cursorLen > 0.001f) ? cursorDiff / cursorLen
                                          : glm::vec2{1, 0};

    // Find nearest enemy
    ctx.hasNearestEnemy = false;
    float nearestDist = std::numeric_limits<float>::max();
    for (auto &enemy : Game::CAT.GetEnemies()) {
        float d = glm::distance(chrPos, enemy->GetPosition());
        if (d < nearestDist) {
            nearestDist = d;
            ctx.nearestEnemy = enemy->GetPosition();
            ctx.hasNearestEnemy = true;
        }
    }

    for (int i = 0; i < amount; i++) {
        auto proj = Resource::GetProjectile(m_ID);
        proj->ResetForReuse();
        proj->SetUp(m_);

        // Set behavior from factory
        auto behavior = CreateBehaviorForWeapon(m_ID);

        // Special handling for StaticFieldAoE: compute offset per projectile
        if (auto *aoe = std::get_if<Projectile::StaticFieldAoE>(&behavior)) {
            float projW = proj->Width() > 0 ? proj->Width() : 50.0f;
            float projH = proj->Height() > 0 ? proj->Height() : 50.0f;
            bool mouseLeft = mouse.x < chrPos.x;
            if (!(i & 1)) {
                // Even index: primary direction
                float xOff = mouseLeft ? -projW / 2 : projW / 2;
                aoe->offset = {xOff, projH / 2 * i};
            } else {
                // Odd index: opposite direction, delayed
                float xOff = mouseLeft ? projW / 2 : -projW / 2;
                aoe->offset = {xOff, -projH / 2 * i};
                proj->SetDelay(static_cast<time_t>(m_["repeatInterval"]));
            }
        }

        // Check if this is a permanent behavior (VESPERS)
        if (auto *orbit = std::get_if<Projectile::OrbitalCircle>(&behavior)) {
            if (orbit->permanent) {
                m_PermanentFired = true;
            }
        }

        proj->SetBehavior(std::move(behavior));

        ctx.projectileIndex = i;
        proj->StartBehavior(ctx);

        // Delay between projectiles in burst (for non-AoE)
        if (i > 0 && m_.count("repeatInterval") &&
            !std::holds_alternative<Projectile::StaticFieldAoE>(
                proj->GetBehavior())) {
            proj->SetDelay(
                static_cast<time_t>(m_["repeatInterval"]) * i);
        }

        Game::CAT.AddProjectile(proj);
    }
}

void Weapon::Draw() {
    auto data = ::Util::ConvertToUniformBufferData(
        m_Transform, m_Drawable->GetSize(), m_ZIndex);
    m_Drawable->Draw(data);
}

void Weapon::LevelUp() {
    assert(m_["level"] < m_["maxLevel"]);
    for (auto &stat : m_LevelUpStat[static_cast<int>(m_["level"]) - 1]) {
        // Apply to base stats so RecalculateStat doesn't erase them
        m_Base[stat.first] += stat.second;
    }
    m_["level"] += 1;
    m_Base["level"] = m_["level"];
    RecalculateStat();
}

bool Weapon::IsMaxLevel() { return m_["level"] == m_["maxLevel"]; }
bool Weapon::IsEvo() { return m_["isEvolution"] == 1.0f; }
bool Weapon::CanEvo() {
    if (!IsMaxLevel())
        return false;
    for (auto &evo : m_EvoRequired) {
        if (!Game::CAT.Have(evo))
            return false;
    }
    for (auto &evo : m_EvoFrom) {
        if (!Game::CAT.Have(evo))
            return false;
    }
    return true;
}

void Weapon::RecalculateStat() {
    m_["power"] = m_Base["power"] * (1.0f + Passive::Passive::GetEffect("power"));
    m_["area"] = m_Base["area"] * (1.0f + Passive::Passive::GetEffect("area"));
    m_["speed"] = m_Base["speed"] * (1.0f + Passive::Passive::GetEffect("speed"));
    m_["amount"] = m_Base["amount"] + Passive::Passive::GetEffect("amount");
    m_["duration"] = m_Base["duration"] * (1.0f + Passive::Passive::GetEffect("duration"));
    m_["coolDown"] = m_Base["interval"] * (1.0f - Passive::Passive::GetEffect("coolDown"));
    m_["interval"] = m_["coolDown"]; // alias for the cooldown timer check
}

void Weapon::UpdateModifier(
    std::unordered_map<std::string, float_t> &modifier) {
    m_Mod = modifier;
    RecalculateStat();
}
} // namespace Game::Weapon
