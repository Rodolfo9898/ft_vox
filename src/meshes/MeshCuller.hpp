#ifndef MESH_CULLER_HPP
# define MESH_CULLER_HPP

#include "../../Libft/Modules/Voxel/voxel.hpp"

#include "../../Libft/Modules/Game/game_voxel_chunk.hpp"

#include "../../src/camera/Camera.hpp"
#include "../../src/chunks/WorldChunk.hpp"
#include "../../src/frame/RenderCache.hpp"
#include "../../Libft/Modules/Voxel/voxel_mesh.hpp"
#include <cmath>

class MeshCuller
{
    public:
        MeshCuller();
        MeshCuller(const MeshCuller &other);
        ~MeshCuller();
        MeshCuller &operator=(const MeshCuller &other);

        static bool chunk_is_visible(const Camera &camera,
            const WorldChunk &world_chunk, const RenderCache &cache);
        static bool triangle_faces_camera(const Camera &camera,
            const WorldChunk &world_chunk,
            const chunk_mesh_vertex triangle_vertices[3]);
};

#endif
