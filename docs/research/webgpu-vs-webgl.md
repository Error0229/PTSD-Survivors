# WebGPU vs WebGL 2.0 — Decision Record

**Decision**: Use **WebGL 2.0** (via OpenGL ES 3.0 + Emscripten) for browser rendering.

## Why Not WebGPU

| Factor | WebGPU | WebGL 2.0 |
|---|---|---|
| Browser coverage (2026) | ~70% | ~97% |
| 2D sprite performance | Overkill at our scale | Handles 10,000+ sprites at 60 FPS |
| Tooling maturity | API still shifting | Battle-tested for years |
| SDL2 integration | Not supported | Native Emscripten support |
| Learning curve | New verbose API + WGSL shaders | Standard GLSL, well-documented |

For a VS clone with ~500-2000 sprites, WebGL 2.0 is more than sufficient. WebGPU's advantages (compute shaders, reduced draw call overhead) only matter at 10,000+ entities with complex GPU logic.

## Why Not Vulkan → WebGPU

**No Vulkan-to-WebGPU translation layer exists.** Confirmed by Emscripten issue #15623. You cannot write Vulkan code and compile it to run in a browser.

The viable cross-platform path is `webgpu.h` API → Dawn (desktop, routes to Vulkan/Metal/D3D12) + emdawnwebgpu (browser). But this adds significant complexity for no practical benefit at our game's scale.

## When to Reconsider

- When WebGPU browser coverage exceeds 90%
- If the game pushes to 10,000+ simultaneous entities with GPU compute
- If PTSD framework adopts `webgpu.h` as its graphics abstraction (then we'd get it for free)

## The PTSD Vulkan Opportunity

If contributing Vulkan support to PTSD, consider proposing `webgpu.h` as the abstraction layer:
- Desktop: Dawn → Vulkan/Metal/D3D12
- Web: emdawnwebgpu → browser WebGPU
- One rendering API, all platforms

This would be a more impactful contribution than raw Vulkan alone, and would make PTSD web-ready (which it currently isn't).

## Sources

- [WebGPU browser support (caniuse.com)](https://caniuse.com/webgpu)
- [Emscripten Vulkan discussion (#15623)](https://github.com/emscripten-core/emscripten/issues/15623)
- [SDL3 GPU API has no WebGPU backend (SDL #10768)](https://github.com/libsdl-org/SDL/issues/10768)
- [WebGPU vs WebGL perf (Toji.dev)](https://toji.dev/webgpu-best-practices/webgl-performance-comparison.html)
- [2D particle benchmark: WebGPU 13x faster, but only at millions of particles](https://www.diva-portal.org/smash/get/diva2:1945245/FULLTEXT02)
- [Dawn WebGPU](https://github.com/google/dawn)
- [emdawnwebgpu for Emscripten](https://dawn.googlesource.com/dawn/+/refs/heads/main/src/emdawnwebgpu/pkg/README.md)
