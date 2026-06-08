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
macOS key-event beep suppression, and 160-cube fullscreen visibility,
has been removed from the active checklist.

## Mandatory Blockers

### Stability/Performance

- Current GPU path renders at ~20 fps on macOS; target is a stable 60 fps
  minimum. Profile and optimise the render loop, chunk mesh generation, and
  draw-call overhead.
- Run long-duration fullscreen tests and track memory growth. The program must
  run for hours without consuming unbounded RAM or slowing down.
- Re-test after performance work on Linux and macOS: no segfaults, aborts,
  uncaught exceptions, freezes, or frame spikes.
- Measure high-speed x20 movement in real GUI mode with terrain streaming
  enabled.
- Confirm `make`, `make -j4`, clean, and rebuild paths still work after renderer
  and input changes.

### Camera/Input

- Linux FPS mouse capture is still unresolved from testing: the cursor can move
  freely instead of being locked/recentered for infinite mouse look. This must
  be fixed and verified on Linux.
- Tune the normal camera speed against the subject expectation of about
  1 cube/second. The current camera defaults to `2.0`; the x20 boost behavior
  should remain available for evaluation.
- Re-run camera movement checks after the mouse-capture fix, including
  continuous forward/back/left/right movement relative to yaw.

### World/Terrain

- Decide whether lakes/water are part of the final mandatory interpretation and
  implement them if needed for the natural-topography expectation.
- Check ground-level readability so the player is not visually lost when looking
  across terrain.
- Keep the terrain proof hooks green during future renderer/input work:
  `--validate-terrain-determinism`, `--validate-world-scale`, and
  `--validate-caves`.

## Bonus Work

Bonus work is only useful once the mandatory part is clean enough for evaluation.
Some bonus hooks already exist, but they still need final validation:

- Dynamic render distance must stay smooth and must never drop below 14 cubes.
- The FPS/debug overlay should remain readable after performance tuning.
- Block deletion/placement should be kept stable after any further mesh/remesh
  changes.
- Multiple biomes exist in Libft data; verify they are visually distinct enough
  in the final renderer.
- x20 movement needs to stay smooth during real terrain generation and rendering.

## Ownership-Oriented Next Steps

rperez-t:

- profile and optimise to reach 60 fps on macOS;
- fix Linux FPS mouse capture;
- verify fullscreen frame pacing under sustained load.

bvangene:

- decide whether optional lakes/water are needed and implement them if yes.

## Immediate Order

1. Profile the GPU render loop and identify the main bottleneck (draw calls,
   mesh generation, CPU-GPU sync, shader cost).
2. Optimise to reach stable 60 fps minimum.
3. Fix Linux FPS mouse capture and verify the camera feels like a true FPS
   camera.
4. Run long-duration memory/stability tests and real GUI x20 movement tests.
