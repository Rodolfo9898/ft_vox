# ft_vox Remaining Work

This file tracks what is still missing from the current repository state.

## Current State

- The project builds with the existing `Libft` submodule.
- The app opens a DUMB fullscreen window.
- The main loop polls input, updates the camera, renders, and presents frames.
- Mouse look exists on two axes.
- `WASD` / arrow movement exists relative to camera yaw.
- `Space` is wired as jump input.
- `B` is wired as a toggle for x20 boost.
- A basic generated chunk grid is rendered.
- The renderer has an 80 degree FoV projection.
- Chunk streaming APIs are exposed through `ft_vox_world_update_around`.
- Chunks are cached and evicted around the player using a fixed memory limit.
- Chunk generation is budgeted during the live render loop.
- The live loop has a dynamic render distance controller.
- Render distance is clamped to at least `14` cubes.
- A repeatable `--validate-visible-distance` check exists for the mandatory `160` cube distance.
- A lightweight software skybox exists.
- Procedural atlas-style texture sampling exists for current block faces.
- Left mouse can delete the targeted solid block through a short raycast.
- Deleting a border block remeshes the touched chunk and horizontal neighbor chunks.
- Libft terrain generation already provides multiple blended biomes: plains, hills, desert, snow, and mountains.
- A debug overlay exists in GUI mode for FPS, frame time, visible chunks, loaded chunks, and render distance.
- Headless perf mode skips GUI-only overlay drawing.

## Mandatory Work Still Missing

### Application and Input

- Verify `B` boost toggle behavior manually on macOS and Linux.
- Verify keyboard input works without macOS system beep.
- Verify close button exits cleanly on macOS and Linux.
- Confirm normal movement speed is close to the subject requirement: around `1 cube/second`.
- Confirm x20 boost actually moves around `20 cubes/second`.

### Camera and Player Movement

- Keep jump/gravity behavior stable and smooth.
- Decide whether the camera should only follow terrain height or support free vertical movement during jump.
- Prevent jump/ground logic from producing camera jitter on uneven terrain.
- Add a clear player-height rule so underground/cave movement is predictable.
- Horizontal collision is intentionally not implemented right now.

### Rendering

- Replace procedural atlas-style colors with a real image texture atlas when asset loading is ready.
- Ensure at least two visually distinct textures are used.
- Ensure at least two non-air block types are visibly distinct.
- Polish the current skybox into the final subject-ready skybox and verify junction/artifact behavior.
- Improve frustum culling so it uses reliable chunk or mesh bounds.
- Add stronger visibility optimization for occluded/hidden geometry.
- Keep render smooth at the mandatory `160` cube visible distance; current headless perf is still not enough margin.
- Continue optimizing the software draw path as visible distance and terrain density increase.

### World Integration

- Keep expanding world streaming toward the full subject scale.
- Support the required visitable size: `16384 x 256 x 16384`.
- Persist or regenerate visited terrain deterministically within the chosen cache limit.
- Keep tuning the generation budget and dynamic distance controller against real GUI movement.

### Terrain Generation

- Confirm terrain is deterministic from the same seed.
- Add or verify natural hills and mountains.
- Add or verify caves below ground.
- Avoid simple `rand()` style terrain.
- Add or verify lakes/water if targeted for mandatory polish.
- Make ground-level scanning clear so the player does not feel lost.

### Performance and Stability

- Run long-duration stability tests.
- Check for memory growth over time.
- Check for VRAM/RAM pressure where applicable.
- Avoid freeze frames during movement, especially at x20 boost.
- Add repeatable perf tests for camera movement and chunk generation.
- Validate macOS and Linux builds after each platform/input change.

## Bonus Work Still Missing

Bonus should only be prioritized after the mandatory part is stable.

- Verify smooth rendering at x20 speed under real GUI load on macOS and Linux.
- Improve block deletion polish if needed: range feedback, no-delete protected blocks, and interaction cooldown.
- Add dirty chunk remeshing for any future vertical chunk layout if world height is split later.
- Add biome/debug visualization if evaluators need the biome transitions to be more obvious.

## rperez-t Remaining Responsibilities

- Own DUMB window/input/platform glue.
- Keep camera controls stable.
- Keep boost toggle behavior stable.
- Keep render loop nonblocking.
- Improve frustum culling and rendering performance.
- Keep the dynamic render distance controller stable.
- Replace procedural atlas colors with real texture atlas assets.
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

1. Run GUI testing on macOS and Linux after the streaming/render changes.
2. Profile the software renderer at larger windows and denser terrain.
3. Test x20 movement under GUI load and tune the dynamic render distance thresholds.
4. Replace procedural atlas colors with real texture atlas loading/sampling.
5. Validate the mandatory `160` cube visible distance in an open GUI scene, not only headless.
