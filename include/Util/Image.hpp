#ifndef UTIL_IMAGE_HPP
#define UTIL_IMAGE_HPP

#include "pch.hpp" // IWYU pragma: export

#include <glm/fwd.hpp>

#include "Core/Drawable.hpp"
#include "Core/Program.hpp"
#include "Core/SpriteBatch.hpp"
#include "Core/Texture.hpp"
#include "Core/TextureAtlas.hpp"
#include "Core/UniformBuffer.hpp"
#include "Core/VertexArray.hpp"

#include "Util/AssetStore.hpp"

namespace Util {
/**
 * @class Image
 * @brief A class representing an image.
 *
 * This class encapsulates the properties and behaviors of an image.
 * It includes properties such as texture and surface.
 * It also includes behaviors such as drawing the image.
 *
 * Supports two rendering modes:
 * - Immediate mode: each sprite issues its own draw call (legacy)
 * - Batch mode: sprites are submitted to a SpriteBatch for batched rendering
 */
class Image : public Core::Drawable {
public:
    /**
     * @brief Constructor that takes a file path to the image.
     *
     * @param filepath The file path to the image.
     * @param useAA Flag indicating whether anti-aliasing should be enabled
     * (default is true).
     */
    Image(const std::string &filepath, bool useAA = true);

    /**
     * @brief Constructor that takes a file path to the image.
     *
     * @param filepath The file path to the image.
     */
    explicit Image(const std::string_view &filepath);

    /**
     * @brief Retrieves the size of the image.
     *
     * This function returns the size of the image.
     *
     * @return The size of the image as a vec2(x, y).
     */
    glm::vec2 GetSize() const override { return m_Size; };

    /**
     * @brief Sets the image to the specified file path.
     *
     * This function sets the image to the specified file path.
     *
     * @param filepath The file path to the image.
     */
    void SetImage(const std::string &filepath);

    /**
     * @brief Sets whether anti-aliasing (AA) should be enabled or disabled.
     *
     * @param useAA A boolean value indicating whether anti-aliasing should be
     * enabled (true) or disabled (false).
     *
     * @note This function only sets the internal flag for anti-aliasing and
     * does not directly affect rendering. The actual effect of anti-aliasing
     * depends on the rendering pipeline and the graphics hardware capabilities.
     *
     * @sa https://en.wikipedia.org/wiki/Spatial_anti-aliasing
     */
    void UseAntiAliasing(bool useAA);

    /**
     * @brief Draws the image with a given transform and z-index.
     *
     * This function draws the image at the specified z-index and applies the
     * given transform.
     *
     * @param transform The transform to apply to the image.
     * @param zIndex The z-index at which to draw the image.
     */
    void Draw(const Core::Matrices &data) override;

    // --- Batch mode API ---

    /**
     * @brief Set the active SpriteBatch. When set, Draw() submits to batch.
     * Pass nullptr to revert to immediate mode.
     */
    static void SetBatch(Core::SpriteBatch *batch) { s_Batch = batch; }

    /**
     * @brief Get the GL texture ID (standalone or atlas).
     */
    GLuint GetTextureId() const;

    /**
     * @brief Build texture atlases from all sprites currently in the texture
     * cache. Call after all sprites are loaded (e.g., after
     * Resource::Initialize). Sprites >256px in either dimension are skipped
     * and keep their standalone textures.
     */
    static void BuildAtlases();

private:
    void InitProgram();
    void InitVertexArray();
    void InitUniformBuffer();

    static constexpr int UNIFORM_SURFACE_LOCATION = 0;

    static std::unique_ptr<Core::Program> s_Program;
    static std::unique_ptr<Core::VertexArray> s_VertexArray;
    std::unique_ptr<Core::UniformBuffer<Core::Matrices>> m_UniformBuffer;

    static Util::AssetStore<std::shared_ptr<SDL_Surface>> s_Store;

    // Shared texture cache: same filepath → same GL texture
    static std::unordered_map<std::string, std::shared_ptr<Core::Texture>>
        s_TextureCache;

    // Active SpriteBatch (null = immediate mode)
    static Core::SpriteBatch *s_Batch;

    // Global atlas lookup: filepath → (atlas textureId, UV region)
    static std::vector<std::unique_ptr<Core::TextureAtlas>> s_Atlases;
    static std::unordered_map<std::string, std::pair<GLuint, Core::AtlasRegion>>
        s_AtlasLookup;

private:
    std::shared_ptr<Core::Texture> m_Texture = nullptr;

    std::string m_Path;
    glm::vec2 m_Size;
};
} // namespace Util

#endif
