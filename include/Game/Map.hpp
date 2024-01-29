#ifndef MAP_HPP
#define MAP_HPP

#include "Util/GameObject.hpp"
#include <glm/fwd.hpp>
#include <memory>
namespace Game {
    class Map : public Util::GameObject {
        public:
        Map() = default;
        Map(std::string mapName);
        ~Map() override = default;
        void Start() override;
        void SetMap(std::string mapName);
        void Update(const Util::Transform &transform = Util::Transform()) override;

        private:
        std::string m_MapName;
        bool m_Enabled = false;
        std::set<std::pair<int32_t, int32_t>> m_VisibleTiles; 
        std::deque <std::unique_ptr<Map>> m_Maps;
    };
}

#endif /* MAP_HPP */
