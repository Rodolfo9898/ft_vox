# ft_vox Remaining Work

Last audit: 2026-06-09

Scope checked: the subject requirements, `main.cpp`, `src/`, and the relevant
Libft modules under `Libft/Modules/DUMB`, `Libft/Modules/Game`, and
`Libft/Modules/Voxel`.

This file only lists work that still needs attention. Completed work, such as
the DUMB window loop, chunk storage, terrain height generation, cave carving,
terrain determinism validation, required-scale validation, texture atlas loading
and correct UV mapping, visible-face mesh generation, frustum culling, skybox
rendering, block texture presentation, debug overlay/FPS counter, block editing
hooks, dynamic render distance, real-size software render target, biome block
data, GPU/OpenGL renderer path, chunk mesh GPU upload via mega-buffer batching,
texture atlas GPU upload, depth testing, macOS key-event beep suppression,
160-cube fullscreen visibility, stable 60+ fps via mega-buffer batching, frustum
and backface culling, Minecraft-like movement speed, jump, and water physics,
lakes with semi-transparent water rendering, water surface-only mesh generation,
terrain faces visible through water, 1-block auto-step water exit, water
spreading on placement, underwater tint, biome visual distinctiveness (stone
biomes vs green/desert), SOLID/OOP menu system with main menu, settings,
credits, loading screen, and in-game settings overlay, random seed on each
Start, AZERTY/QWERTY keyboard layout setting, FPS overlay toggle, render
distance slider, RAM/VRAM usage in overlay, mouse cursor management in-game,
Makefile fclean/make/rebuild paths verified, and terrain proof hooks staying
green through all changes,
has been removed from the active checklist.

## Mandatory Blockers

### Camera/Input

- Linux FPS mouse capture: cursor must be warped to center each frame for
  infinite FPS mouse look (X11 does not warp currently).
- Re-run camera movement checks after the mouse-capture fix, including
  continuous forward/back/left/right movement relative to yaw.

### Stability

- Run long-duration fullscreen tests and track memory growth. The program must
  run for hours without consuming unbounded RAM or slowing down.
- Re-test on Linux: no segfaults, aborts, uncaught exceptions, freezes, or
  frame spikes.
- Measure high-speed x20 movement in real GUI mode with terrain streaming
  enabled.

## Ownership-Oriented Next Steps

rperez-t:

- fix Linux FPS mouse warp and verify infinite mouse look works on Linux;
- run long-duration memory/stability tests on both platforms.

## Immediate Order

1. Fix Linux mouse warp (XWarpPointer to center each frame).
2. Run long-duration stability and x20 movement tests.
3. Confirm all terrain validators remain green.
