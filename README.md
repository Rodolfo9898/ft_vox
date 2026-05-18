# ft_vox

`ft_vox` is a voxel world renderer. The goal is to build a very large deterministic
procedural world, render it smoothly, and let the player move through it with a camera.

This project should mainly use the `Libft` submodule already present in the repository,
especially its DUMB implementation for the application/window/input/rendering layer
where possible.

## Tasks Done

- Added the basic voxel engine support to `Libft`, including the voxel region
  backend. It can be enabled with `#define GAME_USE_VOXEL_REGION_BACKEND 1`.

## Team

- `rperez-t`
- `bvangene`

## Project Goals

Mandatory requirements:

- Generate a deterministic procedural world from a seed.
- Support at least `16384 x 256 x 16384` world space.
- Use textured opaque cubes with at least two block types and two textures.
- Generate natural terrain: hills, mountains, caves, and similar terrain features.
- Save visited terrain in memory up to a defined limit, then evict old terrain.
- Render cubes smoothly with no crashes or long freezes.
- Render at least `160` cubes of visible distance in open areas.
- Implement frustum culling and visibility optimization.
- Use an `80 degree` FoV.
- Add a skybox.
- Add a camera controlled by mouse on two axes.
- Add four movement keys relative to camera rotation.
- Add a speed boost key that multiplies camera speed by `20`.
- Run fullscreen.

Bonus goals:

- Dynamic render distance while preserving smooth display.
- Render distance should never drop below the project minimum in extreme cases.
- FPS counter.
- Smooth render even at `x20` speed.
- Delete blocks with the mouse.
- Multiple biomes.

## Main Rule

Keep world logic and rendering logic separate.

The world module decides what exists.
The renderer module decides how to draw it.

Renderer code should not generate terrain.
World code should not call DUMB, window APIs, or input APIs directly.

## Shared Interfaces

Before implementing large features, agree on the small shared contracts between the two
sides. These names may change during implementation, but the separation should remain.

Suggested shared types:

```cpp
enum BlockType
{
    BLOCK_AIR = 0,
    BLOCK_GRASS,
    BLOCK_DIRT,
    BLOCK_STONE,
    BLOCK_SAND,
    BLOCK_WATER
};

struct Block
{
    BlockType type;
};

struct ChunkCoord
{
    int x;
    int z;
};

struct Camera
{
    // position, rotation, forward/right/up vectors, FoV
};

struct ChunkMesh
{
    // renderable vertices, indices, texture ids, and bounds
};
```

Important dependency direction:

```txt
app/main
  -> input
  -> camera
  -> world
  -> render

world produces ChunkMesh
render consumes ChunkMesh
```

## rperez-t Responsibilities

`rperez-t` owns the application, rendering, input, camera, and frame smoothness side.

Suggested modules:

```txt
src/app/
src/platform/
src/input/
src/camera/
src/render/
src/debug/
```

Mandatory responsibilities:

- Integrate the existing `Libft` submodule into the main executable.
- Use Libft DUMB where possible for windowing, framebuffer/input, and platform glue.
- Create the fullscreen application window.
- Build the main loop.
- Track frame delta time.
- Poll keyboard and mouse input.
- Implement mouse-look camera rotation on two axes.
- Implement forward, backward, left, and right movement.
- Move relative to camera rotation.
- Implement normal speed around `1 cube/second`.
- Implement the `x20` speed boost key.
- Keep FoV fixed at `80 degrees`.
- Render textured cube/chunk meshes received from the world module.
- Implement frustum culling using camera data and chunk bounds.
- Add skybox rendering.
- Avoid frame stalls when world generation is still catching up.

Bonus responsibilities:

- Add FPS counter.
- Add debug display for frame time, visible chunks, loaded chunks, and render distance.
- Implement dynamic render distance control based on performance.
- Add mouse block delete input/raycast trigger.
- Keep camera and rendering smooth at `x20` speed.

rperez-t should not own:

- Terrain generation.
- Chunk cache policy.
- Biome generation.
- Raw block storage layout.
- Chunk mesh generation rules.

## bvangene Responsibilities

`bvangene` owns the world data, terrain generation, chunk loading, mesh creation, and
memory management side.

Suggested modules:

```txt
src/world/
src/chunk/
src/generation/
src/mesh/
src/cache/
src/biome/
```

Mandatory responsibilities:

- Define world coordinates and chunk coordinates.
- Support the required world size of at least `16384 x 256 x 16384`.
- Store block data by chunk.
- Generate chunks deterministically from a seed.
- Generate natural terrain instead of using simple random noise.
- Add hills and mountains.
- Add caves below ground.
- Add at least two non-air block types.
- Maintain visited chunks in memory up to a chosen memory limit.
- Evict old/far chunks after the memory limit is reached.
- Generate chunk meshes from block data.
- Emit only visible cube faces.
- Avoid drawing hidden faces between opaque blocks.
- Provide chunk bounds and mesh data to the renderer.
- Prioritize nearby chunks around the camera.

Bonus responsibilities:

- Add multiple biomes.
- Add terrain transitions such as sand, grass, stone, water, and caves.
- Support block deletion by modifying block data and remeshing dirty chunks.
- Add generation budgets so terrain work does not freeze the frame.
- Improve chunk priority based on camera position and direction.
- Support dynamic render distance requests from the rendering side.

bvangene should not own:

- DUMB window setup.
- Camera movement.
- Keyboard/mouse polling.
- Skybox rendering.
- FPS counter drawing.
- Frame presentation.

## Integration Milestones

### Milestone 1: Base Application

rperez-t:

- Open a fullscreen DUMB window.
- Run a stable main loop.
- Clear/present frames.
- Move a camera in empty space.

bvangene:

- Define `Block`, `ChunkCoord`, and basic chunk storage.
- Generate a flat deterministic test world.
- Produce a simple test `ChunkMesh`.

Integration result:

- The program opens fullscreen and renders a flat voxel surface.

### Milestone 2: Real Terrain

rperez-t:

- Render chunk meshes with basic textures.
- Add FoV, camera projection, and frustum culling.
- Add basic skybox.

bvangene:

- Add seeded terrain height generation.
- Add hills, mountains, caves, and block types.
- Emit visible faces only.

Integration result:

- The player can move through a generated terrain world.

### Milestone 3: Mandatory Performance

rperez-t:

- Keep frame time stable.
- Avoid blocking the render loop.
- Improve culling and draw path.

bvangene:

- Add chunk cache memory limit.
- Add chunk eviction.
- Add generation/remesh budgeting.
- Prioritize visible/nearby chunks.

Integration result:

- Open areas reach the required visible distance smoothly.

### Milestone 4: Bonus

rperez-t:

- Add FPS counter.
- Add dynamic render distance controller.
- Add block delete input/raycast.

bvangene:

- Add biomes.
- Add dirty chunk remeshing after block deletion.
- Improve generation quality and streaming stability.

Integration result:

- Bonus features work without breaking mandatory requirements.

## Working Agreement

- Do not edit the other person's owned modules unless agreed first.
- Shared headers should stay small and stable.
- Prefer adding data to shared structs over making modules call each other directly.
- Keep `Libft` as a submodule; do not vendor external dependencies into this repo.
- If extra libraries are needed, add install/download scripts instead of committing them.
- Keep rendering independent from raw block storage.
- Keep generation independent from DUMB and platform APIs.
- Test integration at the end of every milestone.

## Libft/DUMB Notes

The current plan is to use the existing `Libft` submodule as much as possible. The DUMB
implementation appears to provide platform/render/control pieces that can help with the
windowing and framebuffer side.

Before relying on DUMB for the final evaluated renderer, confirm that it satisfies the
project rule requiring direct use of graphics APIs. If the evaluator requires direct
OpenGL, Vulkan, Metal, or WebGPU usage, keep DUMB behind `src/platform/` so it can be
replaced without rewriting the world module.

## Build

Current build command:

```sh
make
```

Submodule initialization:

```sh
make submodule_init
```

Clean build files:

```sh
make fclean
```
