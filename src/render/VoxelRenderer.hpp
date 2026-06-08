#ifndef VOXEL_RENDERER_HPP
# define VOXEL_RENDERER_HPP

#include "../../src/debug/RenderDebug.hpp"
#include "../../src/debug/DebugOverlayRenderer.hpp"
#include "../../src/frame/RendererColor.hpp"
#include "../../src/sky/SkyboxRenderer.hpp"
#include "../../src/frame/RenderTarget.hpp"
#include "../../src/geometry/ScreenVertex.hpp"
#include "../../src/meshes/ChunkMeshRenderer.hpp"
#include "../../src/frame/RenderCache.hpp"
#include "../../src/assets/TextureAtlas.hpp"
#include "../../src/geometry/TriangleTexture.hpp"
#include "../../Libft/Modules/DUMB/render_window.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include "../../Libft/Modules/Voxel/voxel_mesh.hpp"
#include "../../src/camera/Camera.hpp"
#include "../../src/world/World.hpp"
#include "../../src/gpur/GpuRenderer.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <vector>

class VoxelRenderer
{
    public:
        VoxelRenderer();
        VoxelRenderer(const VoxelRenderer &other);
        ~VoxelRenderer();
        VoxelRenderer &operator=(const VoxelRenderer &other);

        void render_world(ft_render_framebuffer &framebuffer,
            const Camera &camera, const World &world, double display_fps);
        void render_world(ft_render_framebuffer &framebuffer,
            const Camera &camera, const World &world,
            const RenderDebug *debug);
        void preload_assets();
        void initialize_gpu(int width, int height);

    private:
        static const int OVERLAY_W = 512;
        static const int OVERLAY_H = 200;
        uint32_t              _overlay_buf[OVERLAY_W * OVERLAY_H];
        ft_render_framebuffer _overlay_fb;
        uint64_t              _overlay_frame;
        RenderTarget      target;
        ChunkMeshRenderer mesh_renderer;
        GpuRenderer       gpu;

};

#endif
