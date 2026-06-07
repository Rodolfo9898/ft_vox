#ifndef CHUNK_MESH_RENDERER_HPP
# define CHUNK_MESH_RENDERER_HPP

#include "../../src/meshes/MeshProjector.hpp"

#include "../../src/meshes/MeshCuller.hpp"

#include "../../src/meshes/MeshClipper.hpp"

#include "../../src/camera/Camera.hpp"
#include "../../src/chunks/WorldChunk.hpp"
#include "../../src/frame/RenderCache.hpp"
#include "../../src/geometry/ClipVertex.hpp"
#include "../../src/geometry/ScreenVertex.hpp"
#include "../../src/meshes/PerspectiveTriangleRasterizer.hpp"
#include "../../src/meshes/TriangleRasterizer.hpp"
#include "../../Libft/Modules/DUMB/render_window.hpp"
#include "../../Libft/Modules/Voxel/voxel_mesh.hpp"
#include <vector>

class ChunkMeshRenderer
{
    public:
        ChunkMeshRenderer();
        ChunkMeshRenderer(const ChunkMeshRenderer &other);
        ~ChunkMeshRenderer();
        ChunkMeshRenderer &operator=(const ChunkMeshRenderer &other);

        bool draw_mesh(ft_render_framebuffer &framebuffer,
            std::vector<double> &depth_buffer, const Camera &camera,
            const RenderCache &render_cache, const WorldChunk &world_chunk) const;

    private:
        PerspectiveTriangleRasterizer default_rasterizer;
        const TriangleRasterizer     *rasterizer;

        bool draw_triangle_pair(ft_render_framebuffer &framebuffer,
            std::vector<double> &depth_buffer, const RenderCache &render_cache,
            const chunk_mesh_vertex source_vertices[3],
            const ClipVertex clipped_vertices[4], size_t clipped_count) const;
        bool transform_triangle(const Camera &camera,
            const RenderCache &render_cache, const WorldChunk &world_chunk,
            const chunk_mesh_vertex source_vertices[3],
            ClipVertex transformed_vertices[3]) const;
        static constexpr double NEAR_PLANE = 0.08;
};

#endif
