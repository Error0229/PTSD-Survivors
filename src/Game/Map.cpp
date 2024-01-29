#include "Game/Map.hpp"
#include "Game/Config.hpp"
#include "Util/Image.hpp"
#include <memory>

namespace Game {
    Map::Map(std::string mapName) {
        m_MapName = mapName;
        m_Drawable = std::make_unique<Util::Image>(MAP_SPRITE_PATH + mapName);
    }
    void Map::Start() {
    }
    void Map::SetMap(std::string mapName) {
        m_MapName = mapName;
        for (int i = 0; i < MAP_PADDING_NUM; i++) {
            m_Maps.push_back(std::make_unique<Map>(mapName));
        }
        m_Maps[0]->m_Enabled = true;
        m_Maps[0]->m_Transform.translation = {0, 0};
        m_VisibleTiles.insert({0, 0});
    }
    void Map::Update(const Util::Transform &transform) {
        int dx[12] = { -1, -1, 0, 0, 1, 1, -1, -1, 0, 0, 1, 1 };
        int dy[12] = { 0, -1, -1, -1, -1, 0, 0, 1, 1, 1, 1, 0 };
        int ddx[4] = { -1, 1, -1, 1 };
        int ddy[4] = { -1, -1, 1, 1 };

        auto & pos = m_Transform.translation;
        
        for (int qdr = 0; qdr < 4; qdr ++){

        }

    }
}
