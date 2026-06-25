#include "../../src/meshes/MeshCuller.hpp"

const double MeshCuller::CHUNK_CENTER_X_OFFSET = static_cast<double>(GAME_VOXEL_CHUNK_WIDTH) * 0.5;
const double MeshCuller::CHUNK_CENTER_Y_OFFSET = static_cast<double>(GAME_VOXEL_CHUNK_HEIGHT) * 0.5;
const double MeshCuller::CHUNK_CENTER_Z_OFFSET = static_cast<double>(GAME_VOXEL_CHUNK_DEPTH) * 0.5;
const double MeshCuller::CHUNK_HALF_EXTENT_X = static_cast<double>(GAME_VOXEL_CHUNK_WIDTH) * 0.5;
const double MeshCuller::CHUNK_HALF_EXTENT_Y = static_cast<double>(GAME_VOXEL_CHUNK_HEIGHT) * 0.5;
const double MeshCuller::CHUNK_HALF_EXTENT_Z = static_cast<double>(GAME_VOXEL_CHUNK_DEPTH) * 0.5;
const double MeshCuller::CHUNK_SPHERE_RADIUS =
    std::sqrt(MeshCuller::CHUNK_HALF_EXTENT_X * MeshCuller::CHUNK_HALF_EXTENT_X +
              MeshCuller::CHUNK_HALF_EXTENT_Y * MeshCuller::CHUNK_HALF_EXTENT_Y +
              MeshCuller::CHUNK_HALF_EXTENT_Z * MeshCuller::CHUNK_HALF_EXTENT_Z);

MeshCuller::MeshCuller()
{
}
MeshCuller::MeshCuller(const MeshCuller &other)
{
    (void)other;
}
MeshCuller::~MeshCuller()
{
}
MeshCuller &MeshCuller::operator=(const MeshCuller &other)
{
    (void)other;
    return *this;
}

void MeshCuller::face_normal(uint8_t face, double &nx, double &ny, double &nz)
{
    nx = 0.0;
    ny = 0.0;
    nz = 0.0;
    if (face == CHUNK_MESH_FACE_WEST)
        nx = -1.0;
    else if (face == CHUNK_MESH_FACE_EAST)
        nx = 1.0;
    else if (face == CHUNK_MESH_FACE_DOWN)
        ny = -1.0;
    else if (face == CHUNK_MESH_FACE_UP)
        ny = 1.0;
    else if (face == CHUNK_MESH_FACE_NORTH)
        nz = -1.0;
    else
        nz = 1.0;
}

bool MeshCuller::triangle_faces_camera(const Camera &camera, const WorldChunk &world_chunk,
                                       const chunk_mesh_vertex triangle_vertices[3])
{
    double nx, ny, nz;
    face_normal(triangle_vertices[0].face, nx, ny, nz);

    double wx = static_cast<double>(world_chunk.world_x) +
                static_cast<double>(triangle_vertices[0].coordinate_x);
    double wy = static_cast<double>(triangle_vertices[0].coordinate_y);
    double wz = static_cast<double>(world_chunk.world_z) +
                static_cast<double>(triangle_vertices[0].coordinate_z);

    double dot = nx * (camera.x - wx) + ny * (camera.y - wy) + nz * (camera.z - wz);
    return dot > 0.0;
}

void MeshCuller::chunk_center_in_view(const Camera &camera, const WorldChunk &wc,
                                      const RenderCache &cache, double &view_x, double &pitched_y,
                                      double &pitched_z)
{
    double cx = static_cast<double>(wc.world_x) + CHUNK_CENTER_X_OFFSET - camera.x;
    double cy = CHUNK_CENTER_Y_OFFSET - camera.y;
    double cz = static_cast<double>(wc.world_z) + CHUNK_CENTER_Z_OFFSET - camera.z;

    view_x = cx * cache.yaw_cos - cz * cache.yaw_sin;
    double vz = cx * cache.yaw_sin + cz * cache.yaw_cos;

    pitched_y = cy * cache.pitch_cos - vz * cache.pitch_sin;
    pitched_z = cy * cache.pitch_sin + vz * cache.pitch_cos;
}

bool MeshCuller::frustum_test(double view_x, double pitched_y, double pitched_z, double radius,
                              const RenderCache &cache)
{
    if (pitched_z < -radius || pitched_z > cache.render_distance + radius)
        return false;
    double hlimit = pitched_z * cache.fov_tangent * cache.aspect_ratio + radius;
    double vlimit = pitched_z * cache.fov_tangent + radius;
    return std::fabs(view_x) <= hlimit && std::fabs(pitched_y) <= vlimit;
}

bool MeshCuller::chunk_is_visible(const Camera &camera, const WorldChunk &world_chunk,
                                  const RenderCache &cache)
{
    if (world_chunk.mesh.vertices.empty() == FT_TRUE)
        return false;
    double view_x, pitched_y, pitched_z;
    chunk_center_in_view(camera, world_chunk, cache, view_x, pitched_y, pitched_z);
    return frustum_test(view_x, pitched_y, pitched_z, CHUNK_SPHERE_RADIUS, cache);
}
