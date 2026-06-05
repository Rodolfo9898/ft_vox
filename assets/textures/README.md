# ft_vox Textures

`voxel_atlas.png` is the generated source 4x4 voxel texture atlas.
`voxel_atlas.bmp` is the runtime copy loaded by the software renderer.

Current renderer status:

- The software renderer loads `voxel_atlas.bmp`.
- If the asset is missing or invalid, the renderer falls back to procedural
  colors instead of crashing.

Generation prompt:

Create a single seamless pixel-art voxel texture atlas for a Minecraft-like
voxel engine. 4 columns by 4 rows, square atlas, each tile clearly separated by
exact grid alignment but no text or labels. Include these tile materials: grass
top, grass side with dirt, dirt, stone, sand, snow, oak log bark, oak leaves,
cactus, dry shrub, mountain rock, cave dark stone, water surface, clay, gravel,
and bedrock. Style: crisp 16-bit pixel art, orthographic flat texture tiles,
tileable patterns, bright readable colors, no lighting gradients across the
atlas, no text, no logos.
