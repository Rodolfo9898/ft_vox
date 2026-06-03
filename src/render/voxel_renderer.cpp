#include "voxel_renderer.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include "../../Libft/Modules/Voxel/voxel_mesh.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

struct ft_vox_screen_vertex
{
    double x;
    double y;
    double depth;
};

static uint32_t ft_vox_rgba(uint8_t red, uint8_t green, uint8_t blue)
{
    return ((static_cast<uint32_t>(red) << 24U)
        | (static_cast<uint32_t>(green) << 16U)
        | (static_cast<uint32_t>(blue) << 8U)
        | 0xFFU);
}

static uint8_t ft_vox_shade_channel(uint8_t channel, double shade)
{
    int32_t value;

    value = static_cast<int32_t>(static_cast<double>(channel) * shade);
    if (value < 0)
        value = 0;
    if (value > 255)
        value = 255;
    return (static_cast<uint8_t>(value));
}

static uint32_t ft_vox_block_color(uint32_t block_id, uint8_t face)
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    double  shade;

    red = 126U;
    green = 112U;
    blue = 88U;
    if (block_id == TERRAIN_GENERATOR_GRASS_BLOCK)
    {
        red = 82U;
        green = 151U;
        blue = 72U;
    }
    else if (block_id == TERRAIN_GENERATOR_DIRT_BLOCK)
    {
        red = 132U;
        green = 91U;
        blue = 54U;
    }
    else if (block_id == TERRAIN_GENERATOR_STONE_BLOCK)
    {
        red = 118U;
        green = 122U;
        blue = 128U;
    }
    else if (block_id == TERRAIN_GENERATOR_SHRUB_BLOCK)
    {
        red = 61U;
        green = 138U;
        blue = 66U;
    }
    else if (block_id == TERRAIN_GENERATOR_OAK_LOG_BLOCK)
    {
        red = 120U;
        green = 78U;
        blue = 42U;
    }
    else if (block_id == TERRAIN_GENERATOR_OAK_LEAVES_BLOCK)
    {
        red = 46U;
        green = 116U;
        blue = 57U;
    }
    else if (block_id == TERRAIN_GENERATOR_CACTUS_BLOCK)
    {
        red = 43U;
        green = 126U;
        blue = 82U;
    }
    shade = 0.72;
    if (face == CHUNK_MESH_FACE_UP)
        shade = 1.0;
    else if (face == CHUNK_MESH_FACE_NORTH || face == CHUNK_MESH_FACE_WEST)
        shade = 0.58;
    else if (face == CHUNK_MESH_FACE_EAST)
        shade = 0.82;
    return (ft_vox_rgba(ft_vox_shade_channel(red, shade),
        ft_vox_shade_channel(green, shade), ft_vox_shade_channel(blue, shade)));
}

static uint32_t ft_vox_sky_color(int32_t y, int32_t height)
{
    double factor;
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    if (height <= 1)
        return (ft_vox_rgba(94U, 164U, 225U));
    factor = static_cast<double>(y) / static_cast<double>(height - 1);
    red = static_cast<uint8_t>(82.0 + (62.0 * factor));
    green = static_cast<uint8_t>(154.0 + (44.0 * factor));
    blue = static_cast<uint8_t>(224.0 + (22.0 * factor));
    return (ft_vox_rgba(red, green, blue));
}

static void ft_vox_clear_frame(ft_render_framebuffer &framebuffer,
    std::vector<double> &depth_buffer)
{
    int32_t x;
    int32_t y;

    y = 0;
    while (y < framebuffer.height)
    {
        x = 0;
        while (x < framebuffer.width)
        {
            framebuffer.pixels[(y * framebuffer.width) + x]
                = ft_vox_sky_color(y, framebuffer.height);
            depth_buffer[static_cast<size_t>((y * framebuffer.width) + x)]
                = 1.0e30;
            x = x + 1;
        }
        y = y + 1;
    }
}

static bool ft_vox_project_vertex(const ft_vox_camera &camera,
    const chunk_mesh_vertex &vertex, int32_t origin_x, int32_t origin_z,
    int32_t framebuffer_width, int32_t framebuffer_height,
    ft_vox_screen_vertex *screen_vertex)
{
    double world_x;
    double world_y;
    double world_z;
    double translated_x;
    double translated_y;
    double translated_z;
    double yaw_cos;
    double yaw_sin;
    double pitch_cos;
    double pitch_sin;
    double view_x;
    double view_y;
    double view_z;
    double pitched_y;
    double pitched_z;
    double focal_length;

    world_x = static_cast<double>(origin_x)
        + static_cast<double>(vertex.coordinate_x);
    world_y = static_cast<double>(vertex.coordinate_y);
    world_z = static_cast<double>(origin_z)
        + static_cast<double>(vertex.coordinate_z);
    translated_x = world_x - camera.x;
    translated_y = world_y - camera.y;
    translated_z = world_z - camera.z;
    yaw_cos = std::cos(camera.yaw);
    yaw_sin = std::sin(camera.yaw);
    view_x = (translated_x * yaw_cos) - (translated_z * yaw_sin);
    view_z = (translated_x * yaw_sin) + (translated_z * yaw_cos);
    view_y = translated_y;
    pitch_cos = std::cos(camera.pitch);
    pitch_sin = std::sin(camera.pitch);
    pitched_y = (view_y * pitch_cos) - (view_z * pitch_sin);
    pitched_z = (view_y * pitch_sin) + (view_z * pitch_cos);
    if (pitched_z <= 0.1)
        return (false);
    focal_length = (static_cast<double>(framebuffer_width) * 0.5)
        / std::tan(40.0 * 0.017453292519943295);
    screen_vertex->x = (static_cast<double>(framebuffer_width) * 0.5)
        + ((view_x / pitched_z) * focal_length);
    screen_vertex->y = (static_cast<double>(framebuffer_height) * 0.52)
        - ((pitched_y / pitched_z) * focal_length);
    screen_vertex->depth = pitched_z;
    return (true);
}

static double ft_vox_edge(const ft_vox_screen_vertex &a,
    const ft_vox_screen_vertex &b, double x, double y)
{
    return (((x - a.x) * (b.y - a.y)) - ((y - a.y) * (b.x - a.x)));
}

static void ft_vox_draw_triangle(ft_render_framebuffer &framebuffer,
    std::vector<double> &depth_buffer, const ft_vox_screen_vertex vertices[3],
    uint32_t color)
{
    double min_x;
    double max_x;
    double min_y;
    double max_y;
    double area;
    int32_t start_x;
    int32_t end_x;
    int32_t start_y;
    int32_t end_y;
    int32_t x;
    int32_t y;
    double weight0;
    double weight1;
    double weight2;
    double depth;
    size_t pixel_index;

    area = ft_vox_edge(vertices[0], vertices[1], vertices[2].x, vertices[2].y);
    if (std::fabs(area) < 0.000001)
        return ;
    min_x = std::min(vertices[0].x, std::min(vertices[1].x, vertices[2].x));
    max_x = std::max(vertices[0].x, std::max(vertices[1].x, vertices[2].x));
    min_y = std::min(vertices[0].y, std::min(vertices[1].y, vertices[2].y));
    max_y = std::max(vertices[0].y, std::max(vertices[1].y, vertices[2].y));
    start_x = static_cast<int32_t>(std::floor(min_x));
    end_x = static_cast<int32_t>(std::ceil(max_x));
    start_y = static_cast<int32_t>(std::floor(min_y));
    end_y = static_cast<int32_t>(std::ceil(max_y));
    if (start_x < 0)
        start_x = 0;
    if (start_y < 0)
        start_y = 0;
    if (end_x >= framebuffer.width)
        end_x = framebuffer.width - 1;
    if (end_y >= framebuffer.height)
        end_y = framebuffer.height - 1;
    y = start_y;
    while (y <= end_y)
    {
        x = start_x;
        while (x <= end_x)
        {
            weight0 = ft_vox_edge(vertices[1], vertices[2],
                static_cast<double>(x) + 0.5, static_cast<double>(y) + 0.5);
            weight1 = ft_vox_edge(vertices[2], vertices[0],
                static_cast<double>(x) + 0.5, static_cast<double>(y) + 0.5);
            weight2 = ft_vox_edge(vertices[0], vertices[1],
                static_cast<double>(x) + 0.5, static_cast<double>(y) + 0.5);
            if ((weight0 >= 0.0 && weight1 >= 0.0 && weight2 >= 0.0)
                || (weight0 <= 0.0 && weight1 <= 0.0 && weight2 <= 0.0))
            {
                weight0 = weight0 / area;
                weight1 = weight1 / area;
                weight2 = weight2 / area;
                depth = (vertices[0].depth * weight0)
                    + (vertices[1].depth * weight1)
                    + (vertices[2].depth * weight2);
                pixel_index = static_cast<size_t>((y * framebuffer.width) + x);
                if (depth > 0.0 && depth < depth_buffer[pixel_index])
                {
                    depth_buffer[pixel_index] = depth;
                    framebuffer.pixels[pixel_index] = color;
                }
            }
            x = x + 1;
        }
        y = y + 1;
    }
}

static void ft_vox_draw_mesh(ft_render_framebuffer &framebuffer,
    std::vector<double> &depth_buffer, const ft_vox_camera &camera,
    const ft_vox_world_chunk &world_chunk)
{
    size_t index;
    chunk_mesh_vertex source_vertices[3];
    ft_vox_screen_vertex projected_vertices[3];
    uint32_t color;

    index = 0U;
    while (index + 2U < world_chunk.mesh.indices.size())
    {
        source_vertices[0] = world_chunk.mesh.vertices[
            world_chunk.mesh.indices[index]];
        source_vertices[1] = world_chunk.mesh.vertices[
            world_chunk.mesh.indices[index + 1U]];
        source_vertices[2] = world_chunk.mesh.vertices[
            world_chunk.mesh.indices[index + 2U]];
        if (ft_vox_project_vertex(camera, source_vertices[0],
                world_chunk.world_x, world_chunk.world_z, framebuffer.width,
                framebuffer.height, &projected_vertices[0]) == true
            && ft_vox_project_vertex(camera, source_vertices[1],
                world_chunk.world_x, world_chunk.world_z, framebuffer.width,
                framebuffer.height, &projected_vertices[1]) == true
            && ft_vox_project_vertex(camera, source_vertices[2],
                world_chunk.world_x, world_chunk.world_z, framebuffer.width,
                framebuffer.height, &projected_vertices[2]) == true)
        {
            color = ft_vox_block_color(source_vertices[0].block_id,
                source_vertices[0].face);
            ft_vox_draw_triangle(framebuffer, depth_buffer, projected_vertices,
                color);
        }
        index = index + 3U;
    }
}

static void ft_vox_draw_crosshair(ft_render_framebuffer &framebuffer)
{
    int32_t center_x;
    int32_t center_y;
    int32_t offset;
    size_t  pixel_index;

    center_x = framebuffer.width / 2;
    center_y = framebuffer.height / 2;
    offset = -8;
    while (offset <= 8)
    {
        if (center_x + offset >= 0 && center_x + offset < framebuffer.width)
        {
            pixel_index = static_cast<size_t>((center_y * framebuffer.width)
                + center_x + offset);
            framebuffer.pixels[pixel_index] = ft_vox_rgba(245U, 245U, 235U);
        }
        if (center_y + offset >= 0 && center_y + offset < framebuffer.height)
        {
            pixel_index = static_cast<size_t>(((center_y + offset)
                * framebuffer.width) + center_x);
            framebuffer.pixels[pixel_index] = ft_vox_rgba(245U, 245U, 235U);
        }
        offset = offset + 1;
    }
}

void ft_vox_render_world(ft_render_framebuffer &framebuffer,
    const ft_vox_camera &camera, const ft_vox_world &world)
{
    static std::vector<double> depth_buffer;
    size_t required_size;
    int32_t chunk_index;

    if (framebuffer.pixels == nullptr || framebuffer.width <= 0
        || framebuffer.height <= 0)
        return ;
    required_size = static_cast<size_t>(framebuffer.width)
        * static_cast<size_t>(framebuffer.height);
    if (depth_buffer.size() != required_size)
        depth_buffer.resize(required_size);
    ft_vox_clear_frame(framebuffer, depth_buffer);
    chunk_index = 0;
    while (chunk_index < world.chunk_count)
    {
        ft_vox_draw_mesh(framebuffer, depth_buffer, camera,
            world.chunks[chunk_index]);
        chunk_index = chunk_index + 1;
    }
    ft_vox_draw_crosshair(framebuffer);
}
