# CLAUDE.md

## What This Project Is

**PTSD-Survivors** is a Vampire Survivors clone targeting **browser deployment** (Emscripten/WASM) and desktop (Windows/macOS/Linux). It's an easter egg for [error0229.github.io](https://error0229.github.io).

## Core Principles

1. **Browser-first**: Emscripten/WASM is the primary target. Desktop is for dev convenience.
2. **Beautiful code**: Clean architecture, meaningful design patterns, no ceremony. Every class justifies its existence.
3. **Composition over inheritance**: Prefer mixins (`Physical`, `Animated`, `Mirrored`) over deep hierarchies.
4. **Data-driven**: Game content (enemies, weapons, waves) defined in JSON, not hardcoded in C++.
5. **Patterns with purpose**: Don't add a Factory if a constructor suffices.

## Tech Stack

- **Language**: C++20
- **Framework**: PTSD (Practical Tools for Simple Design) — upgrade to latest v0.3
- **Rendering**: OpenGL ES 3.0 / WebGL 2.0 (downgraded from GL 4.1 for browser compat)
- **Windowing/Input**: SDL2
- **Audio**: SDL2_mixer
- **Build**: CMake + Emscripten
- **Assets**: PNG sprite sheets, JSON data, MP3/OGG audio

## Key Architecture Rules

- **Core/** depends on: SDL2, OpenGL/GLES, GLM only
- **Util/** depends on: Core/ (never Game/)
- **Game/** depends on: Util/, Core/
- **Game logic never calls GL or SDL directly** — always through Util/ abstractions

## Reference Material

- **Original game** (full feature reference): `C:\Users\Tim\Desktop\OOPL-VS`
- **Portfolio integration**: `D:\Error0229.github.io\secret\`
- **Detailed docs**: See `docs/` folder for roadmap, architecture, and research notes

## Known Bugs

*(All Phase 4 bugs fixed — see TODOS.md for remaining work items)*

## gstack

Use the `/browse` skill from gstack for all web browsing. Never use `mcp__claude-in-chrome__*` tools directly.

Available skills:
- `/office-hours` — Brainstorming and idea validation
- `/plan-ceo-review` — Strategy-level plan review
- `/plan-eng-review` — Architecture-level plan review
- `/plan-design-review` — Design-level plan review
- `/design-consultation` — Create a design system
- `/review` — Pre-landing code review
- `/ship` — Ship workflow (test, review, PR, deploy)
- `/browse` — Headless browser for QA and site testing
- `/qa` — Systematic QA testing with auto-fix
- `/qa-only` — QA report without fixes
- `/design-review` — Visual design audit with fixes
- `/setup-browser-cookies` — Import browser cookies for authenticated testing
- `/retro` — Weekly engineering retrospective
- `/investigate` — Systematic debugging with root cause analysis
- `/document-release` — Post-ship documentation updates
- `/codex` — Second opinion / adversarial code review
- `/careful` — Safety guardrails for destructive commands
- `/freeze` — Restrict edits to a specific directory
- `/guard` — Maximum safety mode (careful + freeze)
- `/unfreeze` — Remove edit restrictions
- `/gstack-upgrade` — Upgrade gstack to latest version
