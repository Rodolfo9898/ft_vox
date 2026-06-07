#include "../../src/meshes/MeshCuller.hpp"

MeshCuller::MeshCuller()
{
}

MeshCuller::MeshCuller(const MeshCuller &other)
{
    *this = other;
}

MeshCuller::~MeshCuller()
{
}

MeshCuller &MeshCuller::operator=(const MeshCuller &other)
{
    (void)other;
    return (*this);
}

bool MeshCuller::triangle_faces_camera(const Camera &camera,
    const WorldChunk &world_chunk, const chunk_mesh_vertex triangle_vertices[3])
{
    double normal_x;
    double normal_y;
    double normal_z;
    double center_x;
    double center_y;
    double center_z;
    double to_camera_x;
    double to_camera_y;
    double to_camera_z;
    uint8_t face;

    face = triangle_vertices[0].face;
    normal_x = 0.0;
    normal_y = 0.0;
    normal_z = 0.0;
    if (face == CHUNK_MESH_FACE_WEST)
        normal_x = -1.0;
    else if (face == CHUNK_MESH_FACE_EAST)
        normal_x = 1.0;
    else if (face == CHUNK_MESH_FACE_DOWN)
        normal_y = -1.0;
    else if (face == CHUNK_MESH_FACE_UP)
        normal_y = 1.0;
    else if (face == CHUNK_MESH_FACE_NORTH)
        normal_z = -1.0;
    else
        normal_z = 1.0;
    center_x = static_cast<double>(world_chunk.world_x)
        + ((static_cast<double>(triangle_vertices[0].coordinate_x)
                + static_cast<double>(triangle_vertices[1].coordinate_x)
                + static_cast<double>(triangle_vertices[2].coordinate_x))
            / 3.0);
    center_y = (static_cast<double>(triangle_vertices[0].coordinate_y)
            + static_cast<double>(triangle_vertices[1].coordinate_y)
            + static_cast<double>(triangle_vertices[2].coordinate_y))
        / 3.0;
    center_z = static_cast<double>(world_chunk.world_z)
        + ((static_cast<double>(triangle_vertices[0].coordinate_z)
                + static_cast<double>(triangle_vertices[1].coordinate_z)
                + static_cast<double>(triangle_vertices[2].coordinate_z))
            / 3.0);
    to_camera_x = camera.x - center_x;
    to_camera_y = camera.y - center_y;
    to_camera_z = camera.z - center_z;
    if ((normal_x * to_camera_x) + (normal_y * to_camera_y)
        + (normal_z * to_camera_z) <= 0.0)
        return (false);
    return (true);
}

bool MeshCuller::chunk_is_visible(const Camera &camera,
    const WorldChunk &world_chunk, const RenderCache &cache)
{
    double center_x;
    double center_y;
    double center_z;
    double delta_x;
    double delta_y;
    double delta_z;
    double view_x;
    double view_y;
    double view_z;
    double pitched_y;
    double pitched_z;
    double half_extent_x;
    double half_extent_y;
    double half_extent_z;
    double radius;
    double horizontal_limit;
    double vertical_limit;

    if (world_chunk.mesh.has_occupied_bounds == FT_TRUE)
    {
        center_x = static_cast<double>(world_chunk.world_x)
            + (static_cast<double>(world_chunk.mesh.occupied_bounds.minimum_x
                    + world_chunk.mesh.occupied_bounds.maximum_x) * 0.5);
        center_y = static_cast<double>(world_chunk.mesh.occupied_bounds.minimum_y
            + world_chunk.mesh.occupied_bounds.maximum_y) * 0.5;
        center_z = static_cast<double>(world_chunk.world_z)
            + (static_cast<double>(world_chunk.mesh.occupied_bounds.minimum_z
                    + world_chunk.mesh.occupied_bounds.maximum_z) * 0.5);
        half_extent_x = static_cast<double>(
            world_chunk.mesh.occupied_bounds.maximum_x
            - world_chunk.mesh.occupied_bounds.minimum_x) * 0.5;
        half_extent_y = static_cast<double>(
            world_chunk.mesh.occupied_bounds.maximum_y
            - world_chunk.mesh.occupied_bounds.minimum_y) * 0.5;
        half_extent_z = static_cast<double>(
            world_chunk.mesh.occupied_bounds.maximum_z
            - world_chunk.mesh.occupied_bounds.minimum_z) * 0.5;
        radius = std::sqrt((half_extent_x * half_extent_x)
            + (half_extent_y * half_extent_y)
            + (half_extent_z * half_extent_z));
    }
    else
    {
        center_x = static_cast<double>(world_chunk.world_x)
            + (static_cast<double>(GAME_VOXEL_CHUNK_WIDTH) * 0.5);
        center_y = static_cast<double>(GAME_VOXEL_CHUNK_HEIGHT) * 0.5;
        center_z = static_cast<double>(world_chunk.world_z)
            + (static_cast<double>(GAME_VOXEL_CHUNK_DEPTH) * 0.5);
        half_extent_x = static_cast<double>(GAME_VOXEL_CHUNK_WIDTH) * 0.5;
        half_extent_y = static_cast<double>(GAME_VOXEL_CHUNK_HEIGHT) * 0.5;
        half_extent_z = static_cast<double>(GAME_VOXEL_CHUNK_DEPTH) * 0.5;
        radius = std::sqrt((half_extent_x * half_extent_x)
            + (half_extent_y * half_extent_y)
            + (half_extent_z * half_extent_z));
    }
    delta_x = center_x - camera.x;
    delta_y = center_y - camera.y;
    delta_z = center_z - camera.z;
    view_x = (delta_x * cache.yaw_cos) - (delta_z * cache.yaw_sin);
    view_z = (delta_x * cache.yaw_sin) + (delta_z * cache.yaw_cos);
    view_y = delta_y;
    pitched_y = (view_y * cache.pitch_cos) - (view_z * cache.pitch_sin);
    pitched_z = (view_y * cache.pitch_sin) + (view_z * cache.pitch_cos);
    if (pitched_z < -radius)
        return (false);
    if (pitched_z > cache.render_distance + radius)
        return (false);
    horizontal_limit = (pitched_z * cache.fov_tangent
            * cache.aspect_ratio) + radius;
    vertical_limit = (pitched_z * cache.fov_tangent) + radius;
    if (std::fabs(view_x) > horizontal_limit
        || std::fabs(pitched_y) > vertical_limit)
        return (false);
    return (true);
}
