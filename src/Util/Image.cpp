#include "Util/Image.hpp"

#include "Util/Logger.hpp"
#include "pch.hpp"

#include "Core/Texture.hpp"
#include "Core/TextureUtils.hpp"
#include "Util/MissingTexture.hpp"

#include "config.hpp"
#include <glm/fwd.hpp>

std::shared_ptr<SDL_Surface> LoadSurface(const std::string &filepath) {
    auto surface = std::shared_ptr<SDL_Surface>(IMG_Load(filepath.c_str()),
                                                SDL_FreeSurface);

    if (surface == nullptr) {
        surface = {Util::GetMissingImageTextureSDLSurface(), SDL_FreeSurface};
        LOG_ERROR("Failed to load image: '{}'", filepath);
        LOG_ERROR("{}", IMG_GetError());
    }

    return surface;
}

namespace Util {
Image::Image(const std::string &filepath, bool useAA)
    : m_Path(filepath) {
    if (s_Program == nullptr) {
        InitProgram();
    }
    if (s_VertexArray == nullptr) {
        InitVertexArray();
    }

    m_UniformBuffer = std::make_unique<Core::UniformBuffer<Core::Matrices>>(
        *s_Program, "Matrices", 0);

    // Check shared texture cache first
    auto cacheIt = s_TextureCache.find(filepath);
    if (cacheIt != s_TextureCache.end()) {
        m_Texture = cacheIt->second;
    } else {
        auto surface = s_Store.Get(filepath);

        if (surface == nullptr) {
            LOG_ERROR("Failed to load image: '{}'", filepath);
            LOG_ERROR("{}", IMG_GetError());
            surface = {GetMissingImageTextureSDLSurface(), SDL_FreeSurface};
        }

        m_Texture = std::make_shared<Core::Texture>(
            Core::SdlFormatToGlFormat(surface->format->format), surface->w,
            surface->h, surface->pixels, useAA);

        s_TextureCache[filepath] = m_Texture;
    }

    auto surface = s_Store.Get(filepath);
    if (surface) {
        m_Size = {surface->w, surface->h};
    } else {
        m_Size = {1, 1};
    }
}

Image::Image(const std::string_view &filepath) : Image(std::string{filepath}) {}

void Image::SetImage(const std::string &filepath) {
    // Check cache for the new image
    auto cacheIt = s_TextureCache.find(filepath);
    if (cacheIt != s_TextureCache.end()) {
        m_Texture = cacheIt->second;
    } else {
        auto surface = s_Store.Get(filepath);
        m_Texture = std::make_shared<Core::Texture>(
            Core::SdlFormatToGlFormat(surface->format->format), surface->w,
            surface->h, surface->pixels, true);
        s_TextureCache[filepath] = m_Texture;
    }

    auto surface = s_Store.Get(filepath);
    if (surface) {
        m_Size = {surface->w, surface->h};
    }
    m_Path = filepath;
}

void Image::UseAntiAliasing(bool useAA) {
    m_Texture->UseAntiAliasing(useAA);
}

void Image::Draw(const Core::Matrices &data) {
    if (s_Batch) {
        // --- Batch mode: submit pre-transformed vertices ---

        // Transform unit quad corners [-0.5,0.5] by the model matrix
        const glm::mat4 &model = data.m_Model;
        glm::vec2 tl = glm::vec2(model * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f));
        glm::vec2 bl = glm::vec2(model * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f));
        glm::vec2 br = glm::vec2(model * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f));
        glm::vec2 tr = glm::vec2(model * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f));

        // Viewport culling: skip sprites entirely outside the screen.
        // Screen coordinates are centered at (0,0), extending to ±half window.
        constexpr float CULL_MARGIN = 200.0f;
        float halfW = static_cast<float>(PTSD_Config::WINDOW_WIDTH) / 2.0f
                       + CULL_MARGIN;
        float halfH = static_cast<float>(PTSD_Config::WINDOW_HEIGHT) / 2.0f
                       + CULL_MARGIN;

        // If all 4 vertices are outside the same edge, the quad is off-screen.
        if ((tl.x < -halfW && bl.x < -halfW && br.x < -halfW && tr.x < -halfW) ||
            (tl.x >  halfW && bl.x >  halfW && br.x >  halfW && tr.x >  halfW) ||
            (tl.y < -halfH && bl.y < -halfH && br.y < -halfH && tr.y < -halfH) ||
            (tl.y >  halfH && bl.y >  halfH && br.y >  halfH && tr.y >  halfH)) {
            return; // fully off-screen, skip
        }

        GLuint texId;
        glm::vec2 uvMin, uvMax;

        // Check global atlas lookup first
        auto atlasIt = s_AtlasLookup.find(m_Path);
        if (atlasIt != s_AtlasLookup.end()) {
            texId = atlasIt->second.first;
            uvMin = {atlasIt->second.second.u0, atlasIt->second.second.v0};
            uvMax = {atlasIt->second.second.u1, atlasIt->second.second.v1};
        } else {
            texId = m_Texture->GetTextureId();
            uvMin = {0.0f, 0.0f};
            uvMax = {1.0f, 1.0f};
        }

        // z-index is stored in the model matrix z translation
        float zIndex = data.m_Model[3][2];

        s_Batch->Draw(texId, tl, bl, br, tr, uvMin, uvMax, zIndex);
    } else {
        // --- Immediate mode (legacy) ---
        m_UniformBuffer->SetData(0, data);

        m_Texture->Bind(UNIFORM_SURFACE_LOCATION);
        s_Program->Bind();
        s_Program->Validate();

        s_VertexArray->Bind();
        s_VertexArray->DrawTriangles();
    }
}

GLuint Image::GetTextureId() const {
    auto atlasIt = s_AtlasLookup.find(m_Path);
    if (atlasIt != s_AtlasLookup.end()) {
        return atlasIt->second.first;
    }
    return m_Texture ? m_Texture->GetTextureId() : 0;
}

void Image::InitProgram() {
    // TODO: Create `BaseProgram` from `Program` and pass it into `Drawable`
    s_Program =
        std::make_unique<Core::Program>(PTSD_ASSETS_DIR "/shaders/Base.vert",
                                        PTSD_ASSETS_DIR "/shaders/Base.frag");
    s_Program->Bind();

    GLint location = glGetUniformLocation(s_Program->GetId(), "surface");
    glUniform1i(location, UNIFORM_SURFACE_LOCATION);
}

void Image::InitVertexArray() {
    s_VertexArray = std::make_unique<Core::VertexArray>();

    // NOLINTBEGIN
    // These are vertex data for the rectangle but clang-tidy has magic
    // number warnings

    // Vertex
    s_VertexArray->AddVertexBuffer(std::make_unique<Core::VertexBuffer>(
        std::vector<float>{
            -0.5F, 0.5F,  //
            -0.5F, -0.5F, //
            0.5F, -0.5F,  //
            0.5F, 0.5F,   //
        },
        2));

    // UV
    s_VertexArray->AddVertexBuffer(std::make_unique<Core::VertexBuffer>(
        std::vector<float>{
            0.0F, 0.0F, //
            0.0F, 1.0F, //
            1.0F, 1.0F, //
            1.0F, 0.0F, //
        },
        2));

    // Index
    s_VertexArray->SetIndexBuffer(
        std::make_unique<Core::IndexBuffer>(std::vector<unsigned int>{
            0, 1, 2, //
            0, 2, 3, //
        }));
    // NOLINTEND
}

std::unique_ptr<Core::Program> Image::s_Program = nullptr;
std::unique_ptr<Core::VertexArray> Image::s_VertexArray = nullptr;

Util::AssetStore<std::shared_ptr<SDL_Surface>> Image::s_Store(LoadSurface);
std::unordered_map<std::string, std::shared_ptr<Core::Texture>>
    Image::s_TextureCache;
Core::SpriteBatch *Image::s_Batch = nullptr;
std::vector<std::unique_ptr<Core::TextureAtlas>> Image::s_Atlases;
std::unordered_map<std::string, std::pair<GLuint, Core::AtlasRegion>>
    Image::s_AtlasLookup;

void Image::BuildAtlases() {
    if (!s_AtlasLookup.empty()) return; // already built

    constexpr int ATLAS_SIZE = 4096;
    constexpr int MAX_SPRITE_DIM = 256; // skip sprites larger than this

    // Collect all loaded sprite paths and their surface dimensions
    struct SpriteInfo {
        std::string path;
        int w, h;
    };
    std::vector<SpriteInfo> candidates;

    for (const auto &[path, texture] : s_TextureCache) {
        auto surface = s_Store.Get(path);
        if (!surface) continue;
        if (surface->w > MAX_SPRITE_DIM || surface->h > MAX_SPRITE_DIM) continue;
        candidates.push_back({path, surface->w, surface->h});
    }

    if (candidates.empty()) {
        LOG_INFO("Image::BuildAtlases: No sprites to atlas");
        return;
    }

    // Sort by height descending for better shelf packing
    std::sort(candidates.begin(), candidates.end(),
              [](const SpriteInfo &a, const SpriteInfo &b) {
                  return a.h > b.h;
              });

    // Pack into atlases
    auto currentAtlas = std::make_unique<Core::TextureAtlas>(ATLAS_SIZE, ATLAS_SIZE);
    int atlasCount = 1;
    int packedCount = 0;

    for (const auto &sprite : candidates) {
        if (!currentAtlas->Add(sprite.path)) {
            // Current atlas full, finalize and start a new one
            currentAtlas->Build();
            s_Atlases.push_back(std::move(currentAtlas));
            currentAtlas = std::make_unique<Core::TextureAtlas>(ATLAS_SIZE, ATLAS_SIZE);
            atlasCount++;

            if (!currentAtlas->Add(sprite.path)) {
                LOG_WARN("Image::BuildAtlases: Sprite '{}' ({}x{}) won't fit",
                         sprite.path, sprite.w, sprite.h);
                continue;
            }
        }
        packedCount++;
    }

    // Finalize last atlas
    if (packedCount > 0 || currentAtlas->GetTextureId() != 0) {
        currentAtlas->Build();
        s_Atlases.push_back(std::move(currentAtlas));
    }

    // Build the lookup table
    for (const auto &atlas : s_Atlases) {
        GLuint texId = atlas->GetTextureId();
        for (const auto &sprite : candidates) {
            if (atlas->Contains(sprite.path)) {
                s_AtlasLookup[sprite.path] = {texId, atlas->Get(sprite.path)};
            }
        }
    }

    int skippedOversized = static_cast<int>(s_TextureCache.size())
                          - static_cast<int>(candidates.size());
    int skippedNoFit = static_cast<int>(candidates.size()) - packedCount;
    LOG_INFO("Image::BuildAtlases: Packed {} sprites into {} atlas(es), "
             "{} skipped (oversized), {} skipped (no fit)",
             packedCount, atlasCount, skippedOversized, skippedNoFit);
}
} // namespace Util
