# ft_vox Remaining Work

This file tracks what is still missing from the current repository state.
Last code audit: 2026-06-05.

## Current State

- The project builds with the existing `Libft` submodule.
- The app opens a DUMB fullscreen window.
- The main loop polls input, updates the camera, renders, and presents frames.
- Mouse look exists on two axes.
- `WASD` / arrow movement exists relative to camera yaw.
- `Space` is wired as jump input.
- `B` is wired as a toggle for x20 boost.
- Normal camera speed is set to `1.0` cube/second.
- Boost movement is the normal camera speed multiplied by `20`.
- `--validate-camera-speed` verifies the normal and boosted movement distances.
- A basic generated chunk grid is rendered.
- The renderer has an 80 degree FoV projection.
- Chunk streaming APIs are exposed through `ft_vox_world_update_around`.
- Chunks are cached and evicted around the player using a fixed memory limit.
- Chunk generation is budgeted during the live render loop.
- The live loop has a dynamic render distance controller.
- Render distance is clamped to at least `14` cubes.
- A repeatable `--validate-visible-distance` check exists for the mandatory `160` cube distance.
- A lightweight software skybox exists.
- A generated texture atlas asset exists at `assets/textures/voxel_atlas.png`,
  and the runtime atlas is loaded from `assets/textures/voxel_atlas.bmp`.
- The renderer falls back to procedural colors if the runtime atlas is missing.
- Renderer assets are preloaded before the main loop to avoid first-frame IO hitches.
- Left mouse can delete the targeted non-air block through a short raycast.
- Middle mouse picks the targeted block as the selected placement block.
- Right mouse places the selected block on the face-adjacent empty cell.
- Deleting a border block remeshes the touched chunk and horizontal neighbor chunks.
- Placing a block remeshes the touched chunk and horizontal neighbor chunks.
- Libft terrain generation already provides multiple blended biomes: plains, hills, desert, snow, and mountains.
- A debug overlay exists in GUI mode for FPS, frame time, visible chunks, loaded chunks, and render distance.
- The GUI debug overlay also shows the selected placement block id.
- Headless perf mode skips GUI-only overlay drawing.
- Headless perf mode supports movement/chunk streaming stress with
  `--perf-move` and x20 movement with `--perf-boost`.
- Default builds use optimized compiler flags; `DEBUG=1` keeps debug symbols.
- Horizontal collision is intentionally not implemented right now.
- Near-camera terrain projection is clamped to reduce see-through holes when
  the camera gets very close to blocks.

## Mandatory Work Still Missing

### Application and Input

- Manual verification still required after each input/platform change:
  `B` boost toggle, keyboard input without macOS system beep, and close button
  shutdown on macOS and Linux.
- Code-level movement speed is covered by `--validate-camera-speed`.
- Manual GUI feel still needs validation on macOS and Linux.

### Camera and Player Movement

- Keep jump/gravity behavior stable and smooth.
- Current rule: the camera follows terrain while grounded and supports free
  vertical movement during jump/fall.
- Prevent jump/ground logic from producing camera jitter on uneven terrain.
- Add a clear player-height rule so underground/cave movement is predictable.
- Horizontal collision is skipped for now by project decision.

### Rendering

- Real texture sampling is wired through `assets/textures/voxel_atlas.bmp`.
- Polish the current skybox into the final subject-ready skybox and verify junction/artifact behavior.
- Frustum culling currently uses chunk/mesh bounds; keep tuning it against GUI
  movement and dense terrain.
- Add stronger visibility optimization for occluded/hidden geometry.
- Headless render, movement, and x20 movement checks are above 60 FPS on the
  current macOS validation machine.
- Keep validating smooth rendering at the mandatory `160` cube visible distance under real GUI load.
- Continue optimizing the software draw path as visible distance and terrain density increase.

### World Integration

- Keep expanding world streaming toward the full subject scale.
- Support the required visitable size: `16384 x 256 x 16384`.
- Persist or regenerate visited terrain deterministically within the chosen cache limit.
- Keep tuning the generation budget and dynamic distance controller against real GUI movement.

### Terrain Generation

- Confirm terrain is deterministic from the same seed.
- Hills, mountains, and multiple biome transitions exist in Libft terrain; keep
  verifying quality in the GUI.
- Verify caves below ground in the current terrain output.
- Terrain should continue avoiding simple `rand()` style generation.
- Add or verify lakes/water if targeted for mandatory polish.
- Make ground-level scanning clear so the player does not feel lost.

### Performance and Stability

- Run long-duration stability tests.
- Check for memory growth over time.
- Check for VRAM/RAM pressure where applicable.
- Avoid freeze frames during movement, especially at x20 boost.
- Repeatable headless movement/chunk-generation perf exists via
  `--perf-test-headless --perf-move` and `--perf-boost`.
- Validate macOS and Linux builds after each platform/input change.

## Bonus Work Still Missing

Bonus should only be prioritized after the mandatory part is stable.

- Verify smooth rendering at x20 speed under real GUI load on macOS and Linux.
- Improve block edit polish if needed: range feedback, no-delete protected
  blocks, and interaction cooldown.
- Dirty remeshing exists for the edited chunk and horizontal chunk borders; add
  vertical-border remeshing only if the world height is split into vertical chunks later.
- Add biome/debug visualization if evaluators need the biome transitions to be more obvious.

## rperez-t Remaining Responsibilities

- Own DUMB window/input/platform glue.
- Keep camera controls stable.
- Keep boost toggle behavior stable.
- Keep render loop nonblocking.
- Improve frustum culling and rendering performance.
- Keep the dynamic render distance controller stable.
- Maintain real texture atlas assets and fallback behavior.
- Polish the skybox into final project quality.
- Add FPS/debug overlays without affecting headless perf hashes.
- Maintain raycast/delete input as world mutation/remesh APIs evolve.

## bvangene Remaining Responsibilities

- Own block/chunk storage.
- Own deterministic terrain generation.
- Own terrain generation quality and coordinate compatibility with the streaming cache.
- Own caves, mountains, biomes, and terrain quality.
- Own mesh generation rules.
- Own dirty chunk remeshing after block edits.
- Provide renderer-facing chunk mesh bounds and visibility-friendly mesh data.

## Immediate Next Steps

1. Build and run GUI testing on macOS and Linux after the block edit/render changes.
2. Test block delete, pick, and place against terrain, shrubs/leaves, and chunk borders.
3. Profile the software renderer at larger windows and denser terrain.
4. Test x20 movement under GUI load and tune the dynamic render distance thresholds if needed.
5. Validate the mandatory `160` cube visible distance in an open GUI scene, not only headless.
