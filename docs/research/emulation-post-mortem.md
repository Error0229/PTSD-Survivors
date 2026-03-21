# BoxedWine Emulation — Post-Mortem

**Attempted**: Running the original Win32/DirectDraw OOPL-VS game in the browser via BoxedWine x86 emulation.

**Result**: Game boots and renders, but runs at 0.1 FPS. Unusable.

## What Worked

- BoxedWine loaded Wine, resolved MFC/UCRT DLLs, launched game.exe
- GDI BitBlt fallback (added to engine code) allowed rendering to canvas when DirectDraw Blt failed
- Game's loading screen appeared and progress bar rendered correctly
- The easter egg infrastructure (D3 hidden node, secret page, blueprint theme) works well

## What Didn't Work

- **Performance**: BoxedWine web = ~33 MHz Pentium equivalent (interpreter only, no JIT for WASM)
- **Asset loading**: 3,000+ BMP files through emulated I/O took over an hour
- **DirectDraw**: `Blt()` from back buffer to primary surface fails in BoxedWine; required GDI fallback
- **Audio**: DirectSound/MCI not functional in stripped Wine filesystem

## Why No Emulator Can Do This

| Emulator | Speed | Win32 DirectDraw? |
|---|---|---|
| BoxedWine (web) | ~33 MHz Pentium, interpreter only | Yes but 0.1 FPS |
| CheerpX | 5-10x native slowdown | Linux only, no Win32 |
| v86 | 44x native slowdown | Via guest OS, too slow |
| retrowin32 | Interpreter only | Basic, too early |

The fundamental issue: **no x86-in-browser emulator has a WASM JIT** that can achieve playable speed for real-time games.

## Lessons Learned

1. Sprite sheets would have fixed loading time, but not the 0.1 FPS runtime
2. GDI BitBlt as a DirectDraw fallback is a useful technique for Wine/emulator compat
3. The `DDBLTFAST_WAIT` vs `DDBLT_WAIT` flag mismatch was a real bug in the original game
4. BoxedWine is viable for turn-based or simple GUI apps, not real-time games

## Artifacts Preserved

The BoxedWine attempt lives at `D:\Error0229.github.io\secret/`. The portfolio easter egg (hidden D3 node, secret page) will be reused for the WASM build.
