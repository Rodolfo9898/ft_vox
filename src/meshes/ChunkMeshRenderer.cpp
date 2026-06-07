#include "../../src/meshes/ChunkMeshRenderer.hpp"

ChunkMeshRenderer::ChunkMeshRenderer()
    : default_rasterizer(), rasterizer(&this->default_rasterizer)
{
}

ChunkMeshRenderer::ChunkMeshRenderer(const ChunkMeshRenderer &other)
    : default_rasterizer(), rasterizer(&this->default_rasterizer)
{
    *this = other;
}

ChunkMeshRenderer::~ChunkMeshRenderer()
{
}

ChunkMeshRenderer &ChunkMeshRenderer::operator=(
    const ChunkMeshRenderer &other)
{
    (void)other;
    this->rasterizer = &this->default_rasterizer;
    return (*this);
}

bool ChunkMeshRenderer::transform_triangle(const Camera &camera,
    const RenderCache &render_cache, const WorldChunk &world_chunk,
    const chunk_mesh_vertex source_vertices[3],
    ClipVertex transformed_vertices[3]) const
{
    return (MeshProjector::transform_vertex(camera, source_vertices[0],
            world_chunk.world_x, world_chunk.world_z, render_cache,
            &transformed_vertices[0])
        && MeshProjector::transform_vertex(camera, source_vertices[1],
            world_chunk.world_x, world_chunk.world_z, render_cache,
            &transformed_vertices[1])
        && MeshProjector::transform_vertex(camera, source_vertices[2],
            world_chunk.world_x, world_chunk.world_z, render_cache,
            &transformed_vertices[2]));
}

bool ChunkMeshRenderer::draw_triangle_pair(ft_render_framebuffer &framebuffer,
    std::vector<double> &depth_buffer, const RenderCache &render_cache,
    const chunk_mesh_vertex source_vertices[3],
    const ClipVertex clipped_vertices[4], size_t clipped_count) const
{
    ScreenVertex projected_vertices[3];
    bool drew_triangle;

    drew_triangle = false;
    if (clipped_count >= 3U
        && MeshProjector::project_vertex(render_cache, clipped_vertices[0],
            &projected_vertices[0]) == true
        && MeshProjector::project_vertex(render_cache, clipped_vertices[1],
            &projected_vertices[1]) == true
        && MeshProjector::project_vertex(render_cache, clipped_vertices[2],
            &projected_vertices[2]) == true)
    {
        this->rasterizer->draw_triangle(framebuffer, depth_buffer,
            projected_vertices, source_vertices[0].block_id,
            source_vertices[0].face);
        drew_triangle = true;
    }
    if (clipped_count == 4U
        && MeshProjector::project_vertex(render_cache, clipped_vertices[0],
            &projected_vertices[0]) == true
        && MeshProjector::project_vertex(render_cache, clipped_vertices[2],
            &projected_vertices[1]) == true
        && MeshProjector::project_vertex(render_cache, clipped_vertices[3],
            &projected_vertices[2]) == true)
    {
        this->rasterizer->draw_triangle(framebuffer, depth_buffer,
            projected_vertices, source_vertices[0].block_id,
            source_vertices[0].face);
        drew_triangle = true;
    }
    return (drew_triangle);
}

bool ChunkMeshRenderer::draw_mesh(ft_render_framebuffer &framebuffer,
    std::vector<double> &depth_buffer, const Camera &camera,
    const RenderCache &render_cache, const WorldChunk &world_chunk) const
{
    size_t index;
    size_t clipped_count;
    chunk_mesh_vertex source_vertices[3];
    ClipVertex transformed_vertices[3];
    ClipVertex clipped_vertices[4];

    if (world_chunk.initialized == false)
        return (false);
    if (MeshCuller::chunk_is_visible(camera, world_chunk, render_cache)
        == false)
        return (false);
    index = 0U;
    while (index + 2U < world_chunk.mesh.indices.size())
    {
        source_vertices[0] = world_chunk.mesh.vertices[
            world_chunk.mesh.indices[index]];
        source_vertices[1] = world_chunk.mesh.vertices[
            world_chunk.mesh.indices[index + 1U]];
        source_vertices[2] = world_chunk.mesh.vertices[
            world_chunk.mesh.indices[index + 2U]];
        if (MeshCuller::triangle_faces_camera(camera, world_chunk,
                source_vertices) == true
            && this->transform_triangle(camera, render_cache, world_chunk,
                source_vertices, transformed_vertices) == true)
        {
            clipped_count = MeshClipper::clip_near(transformed_vertices,
                clipped_vertices);
            (void)this->draw_triangle_pair(framebuffer, depth_buffer,
                render_cache, source_vertices, clipped_vertices,
                clipped_count);
        }
        index = index + 3U;
    }
    return (true);
}
