#ifndef MESH_CULLER_HPP
#define MESH_CULLER_HPP

#ifndef GAME_USE_VOXEL_REGION_BACKEND
#define GAME_USE_VOXEL_REGION_BACKEND
#endif
#include "../ft_vox.hpp"
#include "../../src/camera/Camera.hpp"
#include "../../src/chunks/WorldChunk.hpp"
#include "../../src/frame/RenderCache.hpp"

class MeshCuller
{
  public:
    MeshCuller();
    MeshCuller(const MeshCuller &other);
    ~MeshCuller();
    MeshCuller &operator=(const MeshCuller &other);

    static bool chunk_is_visible(const Camera &camera, const WorldChunk &world_chunk,
                                 const RenderCache &cache);
    static bool triangle_faces_camera(const Camera &camera, const WorldChunk &world_chunk,
                                      const chunk_mesh_vertex triangle_vertices[3]);

  private:
    static const double CHUNK_CENTER_X_OFFSET;
    static const double CHUNK_CENTER_Y_OFFSET;
    static const double CHUNK_CENTER_Z_OFFSET;
    static const double CHUNK_HALF_EXTENT_X;
    static const double CHUNK_HALF_EXTENT_Y;
    static const double CHUNK_HALF_EXTENT_Z;
    static const double CHUNK_SPHERE_RADIUS;

    static void face_normal(uint8_t face, double &nx, double &ny, double &nz);
    static void chunk_center_in_view(const Camera &camera, const WorldChunk &wc,
                                     const RenderCache &cache, double &view_x, double &pitched_y,
                                     double &pitched_z);
    static bool frustum_test(double view_x, double pitched_y, double pitched_z, double radius,
                             const RenderCache &cache);
};

#endif
