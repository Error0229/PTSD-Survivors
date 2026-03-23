#ifndef CORE_TEXTURE_ATLAS_HPP
#define CORE_TEXTURE_ATLAS_HPP

#include "pch.hpp" // IWYU pragma: export

namespace Core {

/**
 * @brief A region within a texture atlas, storing UV coordinates and pixel
 * size.
 */
struct AtlasRegion {
    float u0, v0, u1, v1; // UV coordinates in atlas
    int pixelW, pixelH;   // original sprite dimensions
};

/**
 * @brief Packs multiple sprites into a single GPU texture using shelf-packing.
 *
 * Usage:
 *   1. Call Add() for each sprite path
 *   2. Call Build() to upload to GPU
 *   3. Use Get() to retrieve UV regions for rendering
 *   4. Use Bind() when drawing batched sprites
 */
class TextureAtlas {
public:
    TextureAtlas(int width = 4096, int height = 4096);
    TextureAtlas(const TextureAtlas &) = delete;
    TextureAtlas(TextureAtlas &&other);

    ~TextureAtlas();

    TextureAtlas &operator=(const TextureAtlas &) = delete;
    TextureAtlas &operator=(TextureAtlas &&other);

    /**
     * @brief Add a sprite to the atlas. Must be called before Build().
     * @return true if the sprite was packed successfully, false if no space.
     */
    bool Add(const std::string &filepath);

    /**
     * @brief Upload the packed atlas to the GPU.
     */
    void Build();

    /**
     * @brief Check if a sprite path exists in this atlas.
     */
    bool Contains(const std::string &filepath) const;

    /**
     * @brief Get the UV region for a previously added sprite.
     */
    const AtlasRegion &Get(const std::string &filepath) const;

    /**
     * @brief Bind the atlas texture to a texture slot.
     */
    void Bind(int slot) const;

    /**
     * @brief Get the OpenGL texture ID.
     */
    GLuint GetTextureId() const { return m_TextureId; }

    /**
     * @brief Get atlas dimensions.
     */
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    struct ShelfRow {
        int y;       // top-left y position of this row
        int height;  // row height (tallest sprite in this row)
        int cursorX; // next free x position in this row
    };

    int m_Width;
    int m_Height;
    GLuint m_TextureId = 0;

    std::vector<unsigned char> m_PixelData; // RGBA pixel buffer
    std::vector<ShelfRow> m_Shelves;
    std::unordered_map<std::string, AtlasRegion> m_Regions;

    static constexpr int PADDING = 1; // pixels between sprites to prevent bleed

    bool PackSprite(const std::string &filepath, int spriteW, int spriteH,
                    const unsigned char *pixels, int pitch);
    void BlitPixels(int destX, int destY, int srcW, int srcH,
                    const unsigned char *srcPixels, int srcPitch);
};

} // namespace Core

#endif
