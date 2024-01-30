#include "Game/Map.hpp"
#include "Game/Camera.hpp"
#include "Game/Config.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"
#include <cstdint>
#include <memory>

namespace Game {
Map::Map(std::string name) {
    m_Name = name;
    m_Drawable =
        std::make_unique<Util::Image>(MAP_SPRITE_PATH + name + SPRITE_EXT);
    m_ZIndex = MAP_LAYER;
}
void Map::Start() {}
void Map::Setup(std::string name) {
    m_Name = name;
    for (int i = 0; i < MAP_PADDING_NUM; i++) {
        m_Maps.push_back(std::make_unique<Map>(name));
    }
    m_Maps[0]->m_Enabled = true;
    m_Maps[0]->m_Position = {0, 0};
    m_Height = m_Maps[0]->m_Drawable->GetSize().y;
    m_Width = m_Maps[0]->m_Drawable->GetSize().x;
    m_VisibleTiles.insert({0, 0});
}
void Map::Update(const Util::Transform &transform) {
    int dx[12] = {-1, -1, 0, 0, 1, 1, -1, -1, 0, 0, 1, 1};
    int dy[12] = {0, -1, -1, -1, -1, 0, 0, 1, 1, 1, 1, 0};
    int ddx[4] = {-1, 1, -1, 1};
    int ddy[4] = {-1, -1, 1, 1};
    auto &player_pos = transform.translation;
    m_Position.x = m_Width * lround(player_pos.x / m_Width);
    m_Position.y = m_Height * lround(player_pos.y / m_Height);
    for (int qr = 0; qr < 4; qr++) {
        if (!(ddx[qr] * (player_pos.x - m_Position.x) > 0 &&
              ddy[qr] * (player_pos.y - m_Position.y) > 0)) {
            continue;
        }
        for (auto &padding : m_Maps) {
            if (!padding->m_Enabled) {
                continue;
            }
            if (padding->m_Position == m_Position)
                continue;
            bool in_range = false;
            for (int i = qr * 3; i < qr * 3 + 3; i++) {
                glm::vec2 p(m_Position.x + dx[i] * m_Width,
                            m_Position.y + dy[i] * m_Height);
                if (p == padding->m_Position) {
                    in_range = true;
                }
            }
            if (!in_range) {
                padding->m_Enabled = false;
                m_VisibleTiles.erase(
                    {padding->m_Position.x, padding->m_Position.y});
            }
        }
        for (int i = qr * 3; i < qr * 3 + 3; i++) {
            std::pair p(m_Position.x + dx[i] * m_Width,
                        m_Position.y + dy[i] * m_Height);
            if (m_VisibleTiles.count(p)) {
                continue;
            }
            m_VisibleTiles.insert(p);
            for (auto &padding : m_Maps) {
                if (padding->m_Enabled == false) {
                    padding->m_Position = {p.first, p.second};
                    padding->m_Enabled = true;
                    break;
                }
            }
        }
    }
    for (auto &padding : m_Maps) {
        if (!padding->m_Enabled) {
            continue;
        }
        padding->m_Transform.translation =
            Camera::WorldToScreen(padding->m_Position);
        padding->m_Drawable->Draw(padding->m_Transform, padding->m_ZIndex);
    }
}
} // namespace Game
