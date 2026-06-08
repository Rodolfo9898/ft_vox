# ft_vox Remaining Work

Last audit: 2026-06-08

Scope checked: the subject requirements, `main.cpp`, `src/`, and the relevant
Libft modules under `Libft/Modules/DUMB`, `Libft/Modules/Game`, and
`Libft/Modules/Voxel`.

This file only lists work that still needs attention. Completed work, such as
the DUMB window loop, chunk storage, terrain height generation, cave carving,
terrain determinism validation, required-scale validation, texture atlas loading,
visible-face mesh generation, frustum culling, skybox rendering, block texture
presentation, debug overlay/FPS counter, block editing hooks, dynamic render
distance, real-size software render target, biome block data, GPU/OpenGL
renderer path, chunk mesh GPU upload, texture atlas GPU upload, depth testing,
macOS key-event beep suppression, 160-cube fullscreen visibility, stable 60 fps
minimum (60 fps floor / 90+ fps typical via mega-buffer single-draw-call
batching, frustum culling, backface culling, and overlay throttling), and
Minecraft-like player movement and jump physics,
has been removed from the active checklist.

## Mandatory Blockers

### Camera/Input

- Linux FPS mouse capture is still unresolved from testing: the cursor can move
  freely instead of being locked/recentered for infinite mouse look. This must
  be fixed and verified on Linux.
- Re-run camera movement checks after the mouse-capture fix, including
  continuous forward/back/left/right movement relative to yaw.

### World/Terrain

- Decide whether lakes/water are part of the final mandatory interpretation and
  implement them if needed for the natural-topography expectation.
- Check ground-level readability so the player is not visually lost when looking
  across terrain.
- Keep the terrain proof hooks green during future work:
  `--validate-terrain-determinism`, `--validate-world-scale`, and
  `--validate-caves`.

### Stability

- Run long-duration fullscreen tests and track memory growth. The program must
  run for hours without consuming unbounded RAM or slowing down.
- Re-test on Linux: no segfaults, aborts, uncaught exceptions, freezes, or
  frame spikes.
- Measure high-speed x20 movement in real GUI mode with terrain streaming
  enabled.
- Confirm `make`, `make -j4`, clean, and rebuild paths still work.

## Bonus Work

Bonus work is only useful once the mandatory part is clean enough for evaluation.
Some bonus hooks already exist, but they still need final validation:

- Dynamic render distance must stay smooth and must never drop below 14 cubes.
- The FPS/debug overlay should remain readable under load.
- Block deletion/placement should remain stable.
- Multiple biomes exist in Libft data; verify they are visually distinct enough.
- x20 movement needs to stay smooth during real terrain generation and rendering.

## Ownership-Oriented Next Steps

rperez-t:

- fix Linux FPS mouse capture and verify the camera feels like a true FPS
  camera;
- run long-duration memory/stability tests.

bvangene:

- decide whether optional lakes/water are needed and implement them if yes.

## Immediate Order

1. Fix Linux FPS mouse capture and verify the camera feels like a true FPS
   camera.
2. Run long-duration memory/stability tests and real GUI x20 movement tests.
3. Verify terrain proof hooks remain green.
