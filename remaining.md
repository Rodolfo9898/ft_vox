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
- `B` boost toggle, keyboard input without macOS system beep, and close-button
  shutdown have been manually accepted on the current macOS/Linux checks.
- Normal camera speed is set to `2.0` cubes/second for smoother collision
  response with the current wider player body.
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
- Horizontal player collision rejects movement into solid blocks without
  disabling movement on clear axes.
- Spawn searches nearby terrain and drops the player onto a clear grounded
  column before the main loop starts.
- Jump speed is tuned high enough to climb a one-block ledge while collision
  still rejects walking directly through the block.
- Near-camera terrain projection is clamped to reduce see-through holes when
  the camera gets very close to blocks.

## Mandatory Work Still Missing

### Application and Input

- Revalidate `B` boost toggle, keyboard input, close-button shutdown, and GUI
  feel only after future DUMB/platform/input changes.
- Code-level movement speed remains covered by `--validate-camera-speed`.

### Camera and Player Movement

- Keep jump/gravity behavior stable and smooth.
- Current rule: the camera follows terrain while grounded and supports free
  vertical movement during jump/fall.
- Prevent jump/ground logic from producing camera jitter on uneven terrain.
- Player dimensions now match the normal Minecraft-style body: `1.80` tall,
  `0.60` wide, with the view/eye height at `1.62`.
- Horizontal collision is implemented for solid blocks, and one-block ledge
  climbing is supported through jump plus the unified AABB resolver.
- Collision resolves to the nearest clear point on each movement axis, so the
  player is clamped to block faces instead of clipping through or falling
  between columns.
- Player collision now uses one Minecraft-style AABB resolver for X, Y, and Z
  movement against the same stored voxel blocks used by editing and rendering.
- Falling lands against any solid block under the player footprint instead of
  only the center terrain column, preventing the player from falling between
  neighboring block columns.
- `--validate-collision` also covers a one-block-wide, two-block-high tunnel
  and a one-block-wide vertical drop shaft.

### Rendering

- Real texture sampling is wired through `assets/textures/voxel_atlas.bmp`.
- Renderer triangles are clipped against the near plane before projection so
  close terrain does not stretch across the screen or expose x-ray views of
  blocks behind it.
- Polish the current skybox into the final subject-ready skybox and verify junction/artifact behavior.
- Frustum culling currently uses chunk/mesh bounds; keep tuning it against GUI
  movement and dense terrain.
- Add stronger visibility optimization for occluded/hidden geometry.
- Headless render, movement, and x20 movement checks are above 60 FPS on the
  current macOS validation machine.
- Keep validating smooth rendering at the mandatory `160` cube visible distance under real GUI load.
- Continue optimizing the software draw path as visible distance and terrain density increase.
- GUI debug statistics are scaled up in the software renderer so FPS, frame
  time, visible chunks, loaded chunks, render distance, and selected block are readable.

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
- `--validate-collision` verifies movement from spawn, wall rejection,
  jump-to-ledge behavior, one-block-wide tunnel traversal, and a vertical
  one-block drop shaft.
- `--validate-block-edit` verifies that place/delete updates both block storage
  and the touched chunk mesh immediately.
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
