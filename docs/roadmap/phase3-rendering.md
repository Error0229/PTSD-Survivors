# Phase 3: Rendering Performance

**Goal**: Achieve 60 FPS with hundreds of sprites in WebGL 2.0.

**Prerequisite**: Phase 2 complete (game renders in browser).

---

## Why This Phase Matters

WebGL has significantly higher per-draw-call overhead than native OpenGL. The current rendering model draws each sprite individually:

**Current flow** (per sprite):
1. `GameObject::Draw()` → `Image::Draw(matrices)`
2. Bind program
3. Set uniform buffer (model + projection matrices)
4. Bind texture
5. Draw 2 triangles (6 indices)

With 500 enemies + 50 projectiles + map tiles + character + UI = **600+ draw calls per frame**. On native GL this is fine. On WebGL, each draw call has JS↔WASM↔GPU marshaling overhead.

**Target**: < 50 draw calls per frame via batching.

---

## Stage 3.1: Texture Atlas System

### Problem
Each sprite is a separate PNG file → separate GL texture. Can't batch sprites that use different textures without an atlas.

### Design

**New class**: `Core::TextureAtlas`

```cpp
class TextureAtlas {
public:
    TextureAtlas(int width, int height);

    // Pack a sprite into the atlas, returns source rect in UV coords
    struct Region {
        float u0, v0, u1, v1;  // UV coordinates in atlas
        int pixelW, pixelH;     // original sprite dimensions
    };

    Region Add(const std::string& spritePath);
    Region Get(const std::string& spritePath) const;

    void Build();           // Upload to GPU
    void Bind(int slot);
    GLuint GetTextureId() const;

private:
    // Rectangle bin packing (shelf or skyline algorithm)
    std::unordered_map<std::string, Region> m_Regions;
    std::vector<unsigned char> m_PixelData;
    GLuint m_TextureId;
    int m_Width, m_Height;
};
```

### Implementation Notes
- Use a simple shelf-packing algorithm (sprites sorted by height, packed left-to-right in rows)
- Atlas size: 4096×4096 is safe for WebGL 2.0 (MAX_TEXTURE_SIZE ≥ 4096)
- May need 2-3 atlases: one for enemies, one for character/weapons, one for map tiles
- Build atlases at startup, not at runtime
- 1px padding between sprites to prevent bleed with GL_NEAREST filtering

### Integration with Image/Animation
- `Image` and `Animation` frame loading must go through atlas
- `Image::Draw()` uses atlas UV region instead of full-texture UVs
- Need to modify the vertex data to use atlas UVs per sprite

---

## Stage 3.2: Sprite Batching

### Problem
Even with atlases, drawing 500 sprites = 500 draw calls if done individually.

### Design

**New class**: `Core::SpriteBatch`

```cpp
class SpriteBatch {
public:
    SpriteBatch(int maxSprites = 4096);

    void Begin(const glm::mat4& projection);

    // Queue a sprite for batched drawing
    void Draw(const TextureAtlas& atlas,
              const TextureAtlas::Region& region,
              const glm::vec2& position,
              const glm::vec2& scale,
              float rotation,
              float zIndex);

    void End();   // Sort by texture+z, flush to GPU
    void Flush(); // Issue actual draw calls

private:
    struct SpriteVertex {
        glm::vec2 position;
        glm::vec2 uv;
    };

    struct SpriteInstance {
        GLuint textureId;
        float zIndex;
        SpriteVertex vertices[4];  // quad corners
    };

    std::vector<SpriteInstance> m_Sprites;
    Core::VertexArray m_VAO;
    Core::VertexBuffer m_VBO;
    Core::IndexBuffer m_IBO;
    Core::Program m_Program;
};
```

### Batching Strategy
1. **Begin()**: Set projection matrix, clear sprite buffer
2. **Draw()**: Compute quad vertices (position, rotation, scale, UV), push to buffer
3. **End()**: Sort sprites by (textureId, zIndex), then flush
4. **Flush()**: For each run of sprites sharing the same texture:
   - Upload vertex data to VBO
   - Bind texture
   - `glDrawElements` for the entire batch
   - One draw call per texture per z-layer

### Vertex Format
```
Per vertex (20 bytes):
  position: vec2 (8 bytes)
  uv:       vec2 (8 bytes)

Per quad: 4 vertices = 80 bytes
Max batch: 4096 sprites = 320 KB vertex data (fits in one VBO)
```

### Index Buffer
Pre-build index buffer for max sprites:
```
For sprite i: indices = [i*4+0, i*4+1, i*4+2, i*4+0, i*4+2, i*4+3]
4096 sprites × 6 indices = 24,576 indices (static, uploaded once)
```

---

## Stage 3.3: Z-Ordered Render Queue

### Current
`Renderer::Update()` collects all GameObjects, sorts by z-index, draws individually.

### Target
Replace individual `GameObject::Draw()` calls with SpriteBatch submissions.

**New render flow**:
```
1. Renderer collects all visible GameObjects
2. For each: compute world transform, look up atlas region
3. Submit to SpriteBatch::Draw()
4. SpriteBatch::End() sorts by (texture, z-index) and flushes
```

### Z-Index Layers (from current code)
```cpp
MAP_LAYER        = 1   // Map tiles (few, large)
ENEMY_LAYER      = 2   // Enemies (many, same atlas)
VFX_LAYER        = 3   // Hit effects
CHARACTER_LAYER  = 4   // Player character
PROJECTILE_LAYER = 5   // Projectiles
// UI drawn separately, not batched
```

**Optimization**: Enemies all share one atlas → one draw call for all enemies. Projectiles share another → one draw call. Map tiles share another → one draw call. **Total: ~5-10 draw calls for the game world**.

---

## Stage 3.4: Integration with Existing Code

### Option A: Replace Renderer internals (recommended)
- Keep `GameObject`, `Image`, `Animation` API the same
- Replace `Image::Draw()` internals to submit to SpriteBatch instead of issuing individual draw calls
- `Renderer::Update()` wraps everything in `SpriteBatch::Begin()` / `End()`
- **Least disruption to game code**

### Option B: Parallel render path
- Keep old immediate-mode rendering for desktop
- Add SpriteBatch path behind `#ifdef __EMSCRIPTEN__` or a runtime flag
- More code to maintain, but safer rollback

**Recommendation**: Option A. SpriteBatch works fine on desktop GL too (it's strictly better). No need for two render paths.

### Changes to PTSD framework classes
This likely requires modifying `Util::Renderer` and `Util::Image` in the PTSD fork:

| Class | Change |
|---|---|
| `Renderer` | Owns a `SpriteBatch`, wraps `Update()` in Begin/End |
| `Image` | `Draw()` submits to batch instead of immediate draw |
| `Animation` | `Draw()` submits current frame to batch |
| `GameObject` | No change (delegates to drawable) |

---

## Stage 3.5: Profiling & Optimization

### Metrics to Track
- Draw calls per frame (target: < 50)
- Frame time (target: < 16.6ms for 60 FPS)
- Vertex data uploaded per frame
- Texture binds per frame

### Profiling Tools
- **Browser**: Chrome DevTools → Performance tab, `WEBGL_debug_renderer_info`
- **Desktop**: RenderDoc, `GL_KHR_debug` callbacks (already in `Core::DebugMessageCallback`)
- **In-game**: FPS counter (already exists in Manager)

### Potential Bottlenecks
1. **Too many texture swaps** → combine more sprites into fewer atlases
2. **Vertex upload** → use `GL_DYNAMIC_DRAW` VBO, or ring buffer
3. **Sorting overhead** → bucket sort by z-layer (only 5 layers) instead of full sort
4. **Map rendering** → map tiles are large and few, may not need batching (profile first)

### Stretch Goal: Instanced Rendering
If batching alone isn't enough:
```cpp
glDrawArraysInstanced(GL_TRIANGLES, 0, 6, spriteCount);
```
Pass per-instance data (position, UV, scale) via a separate VBO with `glVertexAttribDivisor(attr, 1)`. Supported in WebGL 2.0 / GLES 3.0.

---

## Estimated Scope

| Stage | New Files | Modified Files | Risk |
|---|---|---|---|
| 3.1 Texture Atlas | 2 (.h/.cpp) | Image, Animation loaders | Medium |
| 3.2 Sprite Batching | 2 (.h/.cpp) | Shaders (batch vertex format) | Medium |
| 3.3 Render Queue | 0 | Renderer | Low |
| 3.4 Integration | 0 | Image, Animation, Renderer | Medium (PTSD fork) |
| 3.5 Profiling | 0 | 0 | Low |
