# ft_vox Remaining Work

Last audit: 2026-06-07

Scope checked: the subject requirements, `main.cpp`, `src/`, and the relevant
Libft modules under `Libft/Modules/DUMB`, `Libft/Modules/Game`, and
`Libft/Modules/Voxel`.

This file only lists work that still needs attention. Completed work, such as
the DUMB window loop, chunk storage, terrain height generation, cave carving,
terrain determinism validation, required-scale validation, texture atlas loading,
visible-face mesh generation, debug overlay/FPS counter, block editing hooks,
dynamic render distance, real-size software render target, and biome block data,
has been removed from the active checklist.

## Mandatory Blockers

### Renderer/API Compliance

- The current renderer is a CPU software framebuffer renderer in
  `src/render/VoxelRenderer.cpp`. The subject requires working directly with
  an approved graphics API: OpenGL, OpenCL, Vulkan, Metal, or WebGPU. DUMB can
  remain as window/input/platform glue, but final cube rendering needs a direct
  API path.
- Chunk meshes and the texture atlas need to be uploaded/drawn through the GPU
  renderer with depth testing and stable frame pacing.
- The CPU/GPU workload still needs to be balanced and measured once the GPU path
  exists.
- The current software fallback now renders at real framebuffer size, which is
  subject-compliant for resolution but too slow to be the final rendering path.

### Rendering Quality

- Prove that open areas render at least 160 cubes in the player field of vision
  during an actual fullscreen run, not only through the current validation hook.
- Keep frustum culling and visible-face meshing, then add stronger visibility or
  draw-distance control if the GPU path still drops frames.
- Polish the skybox so it has no visible junction artifacts.
- Improve texture/color presentation so the world reads as distinct block types
  instead of a mostly single-color debug look.

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

### Stability/Performance

- Run long-duration fullscreen tests and track memory growth. The program must
  run for hours without consuming unbounded RAM or slowing down.
- Re-test after GPU work on Linux and macOS: no segfaults, aborts, uncaught
  exceptions, freezes, or frame spikes.
- Measure high-speed x20 movement in real GUI mode with terrain streaming
  enabled.
- Confirm `make`, `make -j4`, clean, and rebuild paths still work after renderer
  and input changes.

## Bonus Work

Bonus work is only useful once the mandatory part is clean enough for evaluation.
Some bonus hooks already exist, but they still need final validation:

- Dynamic render distance must stay smooth and must never drop below 14 cubes.
- The FPS/debug overlay should remain readable after the GPU renderer replaces
  the software path.
- Block deletion/placement should be kept stable after mesh upload/remesh logic
  moves to the GPU renderer.
- Multiple biomes exist in Libft data; verify they are visually distinct enough
  in the final renderer.
- x20 movement needs to stay smooth during real terrain generation and rendering.

## Ownership-Oriented Next Steps

rperez-t:

- implement the direct graphics API renderer;
- fix Linux FPS mouse capture;
- verify fullscreen frame pacing, skybox, textures, and 160-cube visibility.

bvangene:

- decide whether optional lakes/water are needed and implement them if yes;
- keep chunk eviction/reload and visible-face mesh generation correct;
- support remeshing after block edits for the final renderer.

## Immediate Order

1. Fix Linux FPS mouse capture and verify the camera feels like a true FPS
   camera.
2. Replace the software framebuffer renderer with a direct approved graphics API
   renderer.
3. Upload chunk meshes and the texture atlas through the GPU renderer.
4. Validate 160-cube fullscreen visibility and smooth frame pacing.
5. Run long-duration memory/stability tests and real GUI x20 movement tests.
