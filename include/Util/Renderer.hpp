#ifndef UTIL_Renderer_HPP
#define UTIL_Renderer_HPP

#include <memory>
#include <vector>

#include "Core/SpriteBatch.hpp"
#include "Util/GameObject.hpp"

class App;

namespace Util {
/**
 * @class Renderer
 * @brief A class handling GameObjects' Draw()
 * @see Util::GameObject
 */
class Renderer final {
public:
    /**
     * @brief Parameterized constructor.
     *
     * @param children The GameObject needing to be managed by Renderer.
     */
    Renderer(const std::vector<std::shared_ptr<GameObject>> &children = {});

    /**
     * @brief Add a child to Renderer.
     *
     * @param child The GameObject needing to be managed by Renderer.
     */
    void AddChild(const std::shared_ptr<GameObject> &child);

    /**
     * @brief Add children to Renderer.
     *
     * @param children The GameObjects needing to be managed by Renderer.
     */
    void AddChildren(const std::vector<std::shared_ptr<GameObject>> &children);

    /**
     * @brief Remove the child.
     *
     * @param child The GameObject being removed.
     */
    void RemoveChild(std::shared_ptr<GameObject> child);

    /**
     * @brief Draw children according to their z-index.
     * Uses SpriteBatch for batched rendering when available.
     *
     * @note The user is not recommended to modify this function.
     */
    void Update();

    /**
     * @brief Get the number of draw calls issued in the last frame.
     */
    int GetDrawCallCount() const;

private:
    std::vector<std::shared_ptr<GameObject>> m_Children;
    std::unique_ptr<Core::SpriteBatch> m_Batch;
};
} // namespace Util

#endif
