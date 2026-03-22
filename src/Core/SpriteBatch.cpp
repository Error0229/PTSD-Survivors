#include "Core/SpriteBatch.hpp"

#include "Util/LoadTextFile.hpp"
#include "Util/Logger.hpp"
#include "config.hpp"

namespace Core {

SpriteBatch::SpriteBatch(int maxSprites)
    : m_MaxSprites(maxSprites) {
    m_Sprites.reserve(maxSprites);
    m_VertexData.reserve(static_cast<size_t>(maxSprites) * 4 * 5);

    // Pre-build index buffer: 6 indices per sprite (2 triangles per quad)
    m_Indices.resize(static_cast<size_t>(maxSprites) * 6);
    for (int i = 0; i < maxSprites; ++i) {
        unsigned int base = static_cast<unsigned int>(i) * 4;
        m_Indices[static_cast<size_t>(i) * 6 + 0] = base + 0;
        m_Indices[static_cast<size_t>(i) * 6 + 1] = base + 1;
        m_Indices[static_cast<size_t>(i) * 6 + 2] = base + 2;
        m_Indices[static_cast<size_t>(i) * 6 + 3] = base + 0;
        m_Indices[static_cast<size_t>(i) * 6 + 4] = base + 2;
        m_Indices[static_cast<size_t>(i) * 6 + 5] = base + 3;
    }

    InitGL();
}

SpriteBatch::~SpriteBatch() {
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_IBO) glDeleteBuffers(1, &m_IBO);
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_ProgramId) glDeleteProgram(m_ProgramId);
}

void SpriteBatch::InitGL() {
    // --- Compile batch shader program ---
    auto vertSrc = Util::LoadTextFile(
        std::string(PTSD_ASSETS_DIR) + "/shaders/Batch.vert");
    auto fragSrc = Util::LoadTextFile(
        std::string(PTSD_ASSETS_DIR) + "/shaders/Batch.frag");

    // Prepend version string for GLES 3.0 / GL 4.1
#ifdef __EMSCRIPTEN__
    std::string version = "#version 300 es\nprecision mediump float;\n";
#else
    std::string version = "#version 410 core\n";
#endif
    std::string fullVert = version + vertSrc;
    std::string fullFrag = version + fragSrc;

    auto compileShader = [](GLenum type, const std::string &src) -> GLuint {
        GLuint id = glCreateShader(type);
        const char *cstr = src.c_str();
        glShaderSource(id, 1, &cstr, nullptr);
        glCompileShader(id);

        GLint success;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(id, 512, nullptr, log);
            LOG_ERROR("SpriteBatch shader compile error: {}", log);
        }
        return id;
    };

    GLuint vert = compileShader(GL_VERTEX_SHADER, fullVert);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fullFrag);

    m_ProgramId = glCreateProgram();
    glAttachShader(m_ProgramId, vert);
    glAttachShader(m_ProgramId, frag);
    glLinkProgram(m_ProgramId);

    GLint success;
    glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(m_ProgramId, 512, nullptr, log);
        LOG_ERROR("SpriteBatch program link error: {}", log);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);

    m_VPUniformLoc = glGetUniformLocation(m_ProgramId, "viewProjection");
    m_SurfaceUniformLoc = glGetUniformLocation(m_ProgramId, "surface");

    // --- Create VAO, VBO, IBO ---
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_IBO);

    glBindVertexArray(m_VAO);

    // VBO: dynamic, will be updated each frame
    // Vertex format: vec3 position (x,y,z) + vec2 uv = 5 floats per vertex
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(m_MaxSprites) * 4 * 5 * sizeof(float),
                 nullptr, GL_DYNAMIC_DRAW);

    // Position: location 0, vec3 (x, y, z)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          reinterpret_cast<void *>(0));

    // UV: location 1, vec2
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          reinterpret_cast<void *>(3 * sizeof(float)));

    // IBO: static, pre-built
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(m_Indices.size() * sizeof(unsigned int)),
                 m_Indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    LOG_INFO("SpriteBatch: Initialized (max {} sprites, VAO={}, VBO={}, IBO={})",
             m_MaxSprites, m_VAO, m_VBO, m_IBO);
}

void SpriteBatch::Begin(const glm::mat4 &viewProjection) {
    m_ViewProjection = viewProjection;
    m_Sprites.clear();
    m_DrawCallCount = 0;
    m_Begun = true;
}

void SpriteBatch::Draw(GLuint textureId,
                        const glm::vec2 &topLeft,
                        const glm::vec2 &bottomLeft,
                        const glm::vec2 &bottomRight,
                        const glm::vec2 &topRight,
                        const glm::vec2 &uvMin,
                        const glm::vec2 &uvMax,
                        float zIndex) {
    if (!m_Begun) {
        LOG_ERROR("SpriteBatch::Draw called without Begin()");
        return;
    }

    if (static_cast<int>(m_Sprites.size()) >= m_MaxSprites) {
        // Flush current batch and continue
        End();
        Begin(m_ViewProjection);
    }

    SpriteEntry entry;
    entry.textureId = textureId;
    entry.zIndex = zIndex;

    // TL — z-index carried through for depth buffer
    entry.vertices[0] = {{topLeft.x, topLeft.y, zIndex}, {uvMin.x, uvMin.y}};
    // BL
    entry.vertices[1] = {{bottomLeft.x, bottomLeft.y, zIndex}, {uvMin.x, uvMax.y}};
    // BR
    entry.vertices[2] = {{bottomRight.x, bottomRight.y, zIndex}, {uvMax.x, uvMax.y}};
    // TR
    entry.vertices[3] = {{topRight.x, topRight.y, zIndex}, {uvMax.x, uvMin.y}};

    m_Sprites.push_back(entry);
}

void SpriteBatch::End() {
    if (!m_Begun) return;
    m_Begun = false;

    if (m_Sprites.empty()) return;

    // Stable sort by z-index first (painter's algorithm), then by texture to
    // batch within the same z-layer. stable_sort preserves submission order for
    // sprites sharing the same z-layer and texture, preventing flicker when
    // overlapping same-layer actors (e.g., enemies) permute between frames.
    std::stable_sort(m_Sprites.begin(), m_Sprites.end(),
                     [](const SpriteEntry &a, const SpriteEntry &b) {
                         if (a.zIndex != b.zIndex)
                             return a.zIndex < b.zIndex;
                         return a.textureId < b.textureId;
                     });

    // Build interleaved vertex data: vec3 pos + vec2 uv = 5 floats per vertex
    m_VertexData.clear();
    m_VertexData.reserve(m_Sprites.size() * 4 * 5);

    for (const auto &sprite : m_Sprites) {
        for (int v = 0; v < 4; ++v) {
            m_VertexData.push_back(sprite.vertices[v].position.x);
            m_VertexData.push_back(sprite.vertices[v].position.y);
            m_VertexData.push_back(sprite.vertices[v].position.z);
            m_VertexData.push_back(sprite.vertices[v].uv.x);
            m_VertexData.push_back(sprite.vertices[v].uv.y);
        }
    }

    // Upload vertex data
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    static_cast<GLsizeiptr>(m_VertexData.size() * sizeof(float)),
                    m_VertexData.data());

    // Bind shader
    glUseProgram(m_ProgramId);
    glUniformMatrix4fv(m_VPUniformLoc, 1, GL_FALSE, &m_ViewProjection[0][0]);
    glUniform1i(m_SurfaceUniformLoc, 0);

    // Flush batches per texture
    GLuint currentTex = m_Sprites[0].textureId;
    int batchStart = 0;

    for (int i = 1; i <= static_cast<int>(m_Sprites.size()); ++i) {
        bool isLast = (i == static_cast<int>(m_Sprites.size()));
        bool texChanged = !isLast && m_Sprites[i].textureId != currentTex;

        if (isLast || texChanged) {
            int count = i - batchStart;
            Flush(currentTex, batchStart, count);

            if (!isLast) {
                currentTex = m_Sprites[i].textureId;
                batchStart = i;
            }
        }
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void SpriteBatch::Flush(GLuint textureId, int startIdx, int count) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    // Force pixel-art filtering to match Core::Texture::Bind() behavior.
    // Without this, standalone (non-atlas) textures may render blurry.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    int indexOffset = startIdx * 6;
    int indexCount = count * 6;

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT,
                   reinterpret_cast<void *>(
                       static_cast<intptr_t>(indexOffset) * sizeof(unsigned int)));

    m_DrawCallCount++;
}

} // namespace Core
