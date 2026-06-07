#include "../../src/render/VoxelRenderer.hpp"

VoxelRenderer::VoxelRenderer()
{
}

VoxelRenderer::VoxelRenderer(const VoxelRenderer &other)
{
    *this = other;
}

VoxelRenderer::~VoxelRenderer()
{
}

VoxelRenderer &VoxelRenderer::operator=(const VoxelRenderer &other)
{
    (void)other;
    return (*this);
}


void VoxelRenderer::render_world(ft_render_framebuffer &framebuffer,
    const Camera &camera, const World &world,
    double display_fps)
{
    RenderDebug debug;

    if (display_fps < 0.0)
    {
        this->render_world(framebuffer, camera, world,
            static_cast<const RenderDebug *>(nullptr));
        return ;
    }
    debug.fps = display_fps;
    debug.frame_ms = 0.0;
    debug.visible_chunks = 0;
    debug.loaded_chunks = world.loaded_chunk_count;
    debug.render_distance = world.active_render_distance;
    debug.selected_block_id = 0U;
    this->render_world(framebuffer, camera, world, &debug);
}

void VoxelRenderer::render_world(ft_render_framebuffer &framebuffer,
    const Camera &camera, const World &world,
    const RenderDebug *debug)
{
    ft_render_framebuffer render_target;
    RenderCache render_cache;
    RenderDebug local_debug;
    int32_t chunk_index;

    if (framebuffer.pixels == nullptr || framebuffer.width <= 0
        || framebuffer.height <= 0)
        return ;
    render_target = this->target.prepare(framebuffer);
    render_cache.configure(camera, render_target.width, render_target.height,
        world.active_render_distance);
    local_debug.fps = 0.0;
    local_debug.frame_ms = 0.0;
    local_debug.visible_chunks = 0;
    local_debug.loaded_chunks = world.loaded_chunk_count;
    local_debug.render_distance = world.active_render_distance;
    local_debug.selected_block_id = 0U;
    if (debug != nullptr)
        local_debug = *debug;
    SkyboxRenderer::clear_frame(render_target, this->target.depth_buffer(), camera);
    chunk_index = 0;
    while (chunk_index < world.chunk_count)
    {
        if (this->mesh_renderer.draw_mesh(render_target,
                this->target.depth_buffer(), camera, render_cache,
                world.chunks[chunk_index]) == true)
            local_debug.visible_chunks = local_debug.visible_chunks + 1;
        chunk_index = chunk_index + 1;
    }
    this->target.blit_to(framebuffer);
    if (debug != nullptr)
        DebugOverlayRenderer::draw_debug_overlay(framebuffer, &local_debug);
    DebugOverlayRenderer::draw_crosshair(framebuffer);
}

void VoxelRenderer::preload_assets()
{
    (void)TextureAtlas::instance();
}
