#ifndef CORE_SPRITE_BATCH_HPP
#define CORE_SPRITE_BATCH_HPP

#include "pch.hpp" // IWYU pragma: export

namespace Core {

/**
 * @brief Batches 2D sprites into minimal draw calls by sorting by texture.
 *
 * Usage per frame:
 *   1. Begin(viewProjection)
 *   2. Draw() for each sprite — pass pre-transformed world-space vertices
 *   3. End() — sorts by texture and flushes batched draw calls
 */
class SpriteBatch {
public:
    SpriteBatch(int maxSprites = 4096);
    SpriteBatch(const SpriteBatch &) = delete;
    SpriteBatch(SpriteBatch &&) = delete;

    ~SpriteBatch();

    SpriteBatch &operator=(const SpriteBatch &) = delete;
    SpriteBatch &operator=(SpriteBatch &&) = delete;

    /**
     * @brief Start a new batch frame.
     * @param viewProjection The combined view-projection matrix.
     */
    void Begin(const glm::mat4 &viewProjection);

    /**
     * @brief Submit a sprite for batched drawing.
     *
     * @param textureId The OpenGL texture to use (atlas or standalone).
     * @param topLeft World-space position of top-left vertex (x, y).
     * @param bottomLeft World-space position of bottom-left vertex (x, y).
     * @param bottomRight World-space position of bottom-right vertex (x, y).
     * @param topRight World-space position of top-right vertex (x, y).
     * @param uvMin UV coordinates of top-left corner.
     * @param uvMax UV coordinates of bottom-right corner.
     * @param zIndex Z-index for draw ordering and depth buffer.
     */
    void Draw(GLuint textureId, const glm::vec2 &topLeft,
              const glm::vec2 &bottomLeft, const glm::vec2 &bottomRight,
              const glm::vec2 &topRight, const glm::vec2 &uvMin,
              const glm::vec2 &uvMax, float zIndex);

    /**
     * @brief Sort sprites and issue batched draw calls.
     */
    void End();

    /**
     * @brief Get the number of draw calls issued in the last End().
     */
    int GetDrawCallCount() const { return m_DrawCallCount; }

    /**
     * @brief Get the number of sprites submitted since Begin().
     */
    int GetSpriteCount() const { return static_cast<int>(m_Sprites.size()); }

private:
    struct SpriteVertex {
        glm::vec3 position; // x, y, z (z = zIndex for depth buffer)
        glm::vec2 uv;
    };

    struct SpriteEntry {
        GLuint textureId;
        float zIndex;
        SpriteVertex vertices[4]; // TL, BL, BR, TR
    };

    void InitGL();
    void Flush(GLuint textureId, int startIdx, int count);

    int m_MaxSprites;
    glm::mat4 m_ViewProjection{1.0f};

    std::vector<SpriteEntry> m_Sprites;
    std::vector<float> m_VertexData;     // interleaved pos+uv for upload
    std::vector<unsigned int> m_Indices; // pre-built index buffer

    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    GLuint m_IBO = 0;
    GLuint m_ProgramId = 0;
    GLint m_VPUniformLoc = -1;
    GLint m_SurfaceUniformLoc = -1;

    int m_DrawCallCount = 0;
    bool m_Begun = false;
};

} // namespace Core

#endif
