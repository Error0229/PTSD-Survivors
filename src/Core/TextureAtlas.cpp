#include "Core/TextureAtlas.hpp"

#include "Core/TextureUtils.hpp"
#include "Util/Logger.hpp"

namespace Core {

TextureAtlas::TextureAtlas(int width, int height)
    : m_Width(width),
      m_Height(height) {
    // Allocate RGBA pixel buffer, initialized to transparent black
    m_PixelData.resize(static_cast<size_t>(width) * height * 4, 0);
}

TextureAtlas::TextureAtlas(TextureAtlas &&other)
    : m_Width(other.m_Width),
      m_Height(other.m_Height),
      m_TextureId(other.m_TextureId),
      m_PixelData(std::move(other.m_PixelData)),
      m_Shelves(std::move(other.m_Shelves)),
      m_Regions(std::move(other.m_Regions)) {
    other.m_TextureId = 0;
}

TextureAtlas::~TextureAtlas() {
    if (m_TextureId != 0) {
        glDeleteTextures(1, &m_TextureId);
    }
}

TextureAtlas &TextureAtlas::operator=(TextureAtlas &&other) {
    if (this != &other) {
        if (m_TextureId != 0) {
            glDeleteTextures(1, &m_TextureId);
        }
        m_Width = other.m_Width;
        m_Height = other.m_Height;
        m_TextureId = other.m_TextureId;
        m_PixelData = std::move(other.m_PixelData);
        m_Shelves = std::move(other.m_Shelves);
        m_Regions = std::move(other.m_Regions);
        other.m_TextureId = 0;
    }
    return *this;
}

bool TextureAtlas::Add(const std::string &filepath) {
    if (m_Regions.count(filepath)) {
        return true; // already added
    }

    auto surface = std::shared_ptr<SDL_Surface>(
        IMG_Load(filepath.c_str()), SDL_FreeSurface);
    if (!surface) {
        LOG_ERROR("TextureAtlas: Failed to load '{}'", filepath);
        return false;
    }

    // Convert to RGBA32 for consistent pixel format
    auto converted = std::shared_ptr<SDL_Surface>(
        SDL_ConvertSurfaceFormat(surface.get(), SDL_PIXELFORMAT_RGBA32, 0),
        SDL_FreeSurface);
    if (!converted) {
        LOG_ERROR("TextureAtlas: Failed to convert '{}' to RGBA32", filepath);
        return false;
    }

    SDL_LockSurface(converted.get());
    bool result = PackSprite(
        filepath, converted->w, converted->h,
        static_cast<const unsigned char *>(converted->pixels),
        converted->pitch);
    SDL_UnlockSurface(converted.get());

    if (!result) {
        LOG_WARN("TextureAtlas: No space for '{}' ({}x{})", filepath,
                 converted->w, converted->h);
    }
    return result;
}

bool TextureAtlas::PackSprite(const std::string &filepath,
                               int spriteW, int spriteH,
                               const unsigned char *pixels, int pitch) {
    int paddedW = spriteW + PADDING;
    int paddedH = spriteH + PADDING;

    // Try to fit in an existing shelf
    for (auto &shelf : m_Shelves) {
        if (spriteH <= shelf.height && shelf.cursorX + paddedW <= m_Width) {
            int x = shelf.cursorX;
            int y = shelf.y;

            BlitPixels(x, y, spriteW, spriteH, pixels, pitch);
            shelf.cursorX += paddedW;

            m_Regions[filepath] = AtlasRegion{
                static_cast<float>(x) / m_Width,
                static_cast<float>(y) / m_Height,
                static_cast<float>(x + spriteW) / m_Width,
                static_cast<float>(y + spriteH) / m_Height,
                spriteW, spriteH};
            return true;
        }
    }

    // Create a new shelf
    int shelfY = 0;
    if (!m_Shelves.empty()) {
        auto &last = m_Shelves.back();
        shelfY = last.y + last.height + PADDING;
    }

    if (shelfY + paddedH > m_Height || paddedW > m_Width) {
        return false; // no space
    }

    m_Shelves.push_back(ShelfRow{shelfY, paddedH, paddedW});

    BlitPixels(0, shelfY, spriteW, spriteH, pixels, pitch);

    m_Regions[filepath] = AtlasRegion{
        0.0f,
        static_cast<float>(shelfY) / m_Height,
        static_cast<float>(spriteW) / m_Width,
        static_cast<float>(shelfY + spriteH) / m_Height,
        spriteW, spriteH};
    return true;
}

void TextureAtlas::BlitPixels(int destX, int destY,
                                int srcW, int srcH,
                                const unsigned char *srcPixels, int srcPitch) {
    for (int row = 0; row < srcH; ++row) {
        const unsigned char *srcRow = srcPixels + row * srcPitch;
        unsigned char *destRow = m_PixelData.data() +
                                 (static_cast<size_t>(destY + row) * m_Width +
                                  destX) * 4;
        std::memcpy(destRow, srcRow, static_cast<size_t>(srcW) * 4);
    }
}

void TextureAtlas::Build() {
    if (m_TextureId != 0) {
        glDeleteTextures(1, &m_TextureId);
    }

    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_PixelData.data());

    // GL_NEAREST for pixel-art sprites, no bleed between atlas regions
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#ifdef __EMSCRIPTEN__
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
#endif

    // Free CPU-side pixel data after upload
    m_PixelData.clear();
    m_PixelData.shrink_to_fit();

    LOG_INFO("TextureAtlas: Built {}x{} atlas with {} sprites (GL id: {})",
             m_Width, m_Height, m_Regions.size(), m_TextureId);
}

bool TextureAtlas::Contains(const std::string &filepath) const {
    return m_Regions.count(filepath) > 0;
}

const AtlasRegion &TextureAtlas::Get(const std::string &filepath) const {
    return m_Regions.at(filepath);
}

void TextureAtlas::Bind(int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
}

} // namespace Core
