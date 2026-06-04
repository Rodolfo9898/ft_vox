#include "voxel_renderer.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include "../../Libft/Modules/Voxel/voxel_mesh.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <vector>

struct ft_vox_screen_vertex
{
    double x;
    double y;
    double depth;
    double texture_u;
    double texture_v;
};

struct ft_vox_render_cache
{
    double yaw_cos;
    double yaw_sin;
    double pitch_cos;
    double pitch_sin;
    double focal_length;
    double half_width;
    double half_height;
};

static uint32_t ft_vox_rgba(uint8_t red, uint8_t green, uint8_t blue)
{
    return ((static_cast<uint32_t>(red) << 16U)
        | (static_cast<uint32_t>(green) << 8U)
        | static_cast<uint32_t>(blue));
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
    static std::vector<uint32_t> sky_row_colors;
    int32_t y;
    size_t row_index;

    if (sky_row_colors.size() != static_cast<size_t>(framebuffer.height))
        sky_row_colors.resize(static_cast<size_t>(framebuffer.height));
    y = 0;
    while (y < framebuffer.height)
    {
        sky_row_colors[static_cast<size_t>(y)] = ft_vox_sky_color(y,
            framebuffer.height);
        y = y + 1;
    }
    y = 0;
    while (y < framebuffer.height)
    {
        row_index = static_cast<size_t>(y * framebuffer.width);
        std::fill_n(framebuffer.pixels + row_index, framebuffer.width,
            sky_row_colors[static_cast<size_t>(y)]);
        std::fill_n(depth_buffer.data() + row_index, framebuffer.width, 1.0e30);
        y = y + 1;
    }
}

static bool ft_vox_project_vertex(const ft_vox_camera &camera,
    const chunk_mesh_vertex &vertex, int32_t origin_x, int32_t origin_z,
    const ft_vox_render_cache &render_cache,
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
    yaw_cos = render_cache.yaw_cos;
    yaw_sin = render_cache.yaw_sin;
    view_x = (translated_x * yaw_cos) - (translated_z * yaw_sin);
    view_z = (translated_x * yaw_sin) + (translated_z * yaw_cos);
    view_y = translated_y;
    pitch_cos = render_cache.pitch_cos;
    pitch_sin = render_cache.pitch_sin;
    pitched_y = (view_y * pitch_cos) - (view_z * pitch_sin);
    pitched_z = (view_y * pitch_sin) + (view_z * pitch_cos);
    if (pitched_z <= 0.1)
        return (false);
    focal_length = render_cache.focal_length;
    screen_vertex->x = render_cache.half_width
        + ((view_x / pitched_z) * focal_length);
    screen_vertex->y = render_cache.half_height
        - ((pitched_y / pitched_z) * focal_length);
    screen_vertex->depth = pitched_z;
    screen_vertex->texture_u = static_cast<double>(vertex.texture_u);
    screen_vertex->texture_v = static_cast<double>(vertex.texture_v);
    return (true);
}

static double ft_vox_edge(const ft_vox_screen_vertex &a,
    const ft_vox_screen_vertex &b, double x, double y)
{
    return (((x - a.x) * (b.y - a.y)) - ((y - a.y) * (b.x - a.x)));
}

static double ft_vox_edge_step_x(const ft_vox_screen_vertex &a,
    const ft_vox_screen_vertex &b) noexcept
{
    return (b.y - a.y);
}

static double ft_vox_edge_step_y(const ft_vox_screen_vertex &a,
    const ft_vox_screen_vertex &b) noexcept
{
    return (a.x - b.x);
}

static uint32_t ft_vox_sample_block_texture(uint32_t block_id, uint8_t face,
    double texture_u, double texture_v)
{
    uint32_t color;
    double shade;
    int32_t checker;

    color = ft_vox_block_color(block_id, face);
    checker = (static_cast<int32_t>(std::floor(texture_u * 0.125))
        + static_cast<int32_t>(std::floor(texture_v * 0.125))) & 1;
    shade = checker == 0 ? 1.0 : 0.88;
    if (block_id == TERRAIN_GENERATOR_GRASS_BLOCK
        && face == CHUNK_MESH_FACE_UP)
        shade = checker == 0 ? 1.08 : 0.90;
    else if (block_id == TERRAIN_GENERATOR_OAK_LOG_BLOCK)
        shade = checker == 0 ? 1.12 : 0.72;
    return (ft_vox_rgba(
        ft_vox_shade_channel(static_cast<uint8_t>((color >> 16U) & 0xFFU),
            shade),
        ft_vox_shade_channel(static_cast<uint8_t>((color >> 8U) & 0xFFU),
            shade),
        ft_vox_shade_channel(static_cast<uint8_t>(color & 0xFFU), shade)));
}

static void ft_vox_draw_triangle(ft_render_framebuffer &framebuffer,
    std::vector<double> &depth_buffer, const ft_vox_screen_vertex vertices[3],
    uint32_t block_id, uint8_t face)
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
    double edge0_row;
    double edge1_row;
    double edge2_row;
    double edge0;
    double edge1;
    double edge2;
    double step_x0;
    double step_y0;
    double step_x1;
    double step_y1;
    double step_x2;
    double step_y2;
    double depth;
    double texture_u;
    double texture_v;
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
    step_x0 = ft_vox_edge_step_x(vertices[1], vertices[2]);
    step_y0 = ft_vox_edge_step_y(vertices[1], vertices[2]);
    step_x1 = ft_vox_edge_step_x(vertices[2], vertices[0]);
    step_y1 = ft_vox_edge_step_y(vertices[2], vertices[0]);
    step_x2 = ft_vox_edge_step_x(vertices[0], vertices[1]);
    step_y2 = ft_vox_edge_step_y(vertices[0], vertices[1]);
    edge0_row = ft_vox_edge(vertices[1], vertices[2],
        static_cast<double>(start_x) + 0.5, static_cast<double>(start_y) + 0.5);
    edge1_row = ft_vox_edge(vertices[2], vertices[0],
        static_cast<double>(start_x) + 0.5, static_cast<double>(start_y) + 0.5);
    edge2_row = ft_vox_edge(vertices[0], vertices[1],
        static_cast<double>(start_x) + 0.5, static_cast<double>(start_y) + 0.5);
    y = start_y;
    while (y <= end_y)
    {
        edge0 = edge0_row;
        edge1 = edge1_row;
        edge2 = edge2_row;
        x = start_x;
        while (x <= end_x)
        {
            if ((edge0 >= 0.0 && edge1 >= 0.0 && edge2 >= 0.0)
                || (edge0 <= 0.0 && edge1 <= 0.0 && edge2 <= 0.0))
            {
                depth = (vertices[0].depth * (edge0 / area))
                    + (vertices[1].depth * (edge1 / area))
                    + (vertices[2].depth * (edge2 / area));
                pixel_index = static_cast<size_t>((y * framebuffer.width) + x);
                if (depth > 0.0 && depth < depth_buffer[pixel_index])
                {
                    texture_u = (vertices[0].texture_u * (edge0 / area))
                        + (vertices[1].texture_u * (edge1 / area))
                        + (vertices[2].texture_u * (edge2 / area));
                    texture_v = (vertices[0].texture_v * (edge0 / area))
                        + (vertices[1].texture_v * (edge1 / area))
                        + (vertices[2].texture_v * (edge2 / area));
                    depth_buffer[pixel_index] = depth;
                    framebuffer.pixels[pixel_index] =
                        ft_vox_sample_block_texture(block_id, face,
                            texture_u, texture_v);
                }
            }
            edge0 += step_x0;
            edge1 += step_x1;
            edge2 += step_x2;
            x = x + 1;
        }
        edge0_row += step_y0;
        edge1_row += step_y1;
        edge2_row += step_y2;
        y = y + 1;
    }
}

static ft_bool ft_vox_triangle_faces_camera(
    const ft_vox_camera &camera, const ft_vox_world_chunk &world_chunk,
    const chunk_mesh_vertex triangle_vertices[3]) noexcept
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
        return (FT_FALSE);
    return (FT_TRUE);
}

static bool ft_vox_chunk_is_near_camera(const ft_vox_camera &camera,
    const ft_vox_world_chunk &world_chunk, const ft_vox_render_cache &cache)
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
    double radius;
    double aspect_ratio;
    double horizontal_limit;
    double vertical_limit;

    if (world_chunk.mesh.has_occupied_bounds == FT_TRUE)
    {
        center_x = static_cast<double>(world_chunk.world_x)
            + (static_cast<double>(world_chunk.mesh.occupied_bounds.minimum_x
                    + world_chunk.mesh.occupied_bounds.maximum_x) * 0.5);
        center_y = static_cast<double>(
            world_chunk.mesh.occupied_bounds.minimum_y
            + world_chunk.mesh.occupied_bounds.maximum_y) * 0.5;
        center_z = static_cast<double>(world_chunk.world_z)
            + (static_cast<double>(world_chunk.mesh.occupied_bounds.minimum_z
                    + world_chunk.mesh.occupied_bounds.maximum_z) * 0.5);
        radius = static_cast<double>(
            (world_chunk.mesh.occupied_bounds.maximum_x
                - world_chunk.mesh.occupied_bounds.minimum_x)
            + (world_chunk.mesh.occupied_bounds.maximum_y
                - world_chunk.mesh.occupied_bounds.minimum_y)
            + (world_chunk.mesh.occupied_bounds.maximum_z
                - world_chunk.mesh.occupied_bounds.minimum_z)) * 0.5;
    }
    else
    {
        center_x = static_cast<double>(world_chunk.world_x)
            + (static_cast<double>(GAME_VOXEL_CHUNK_WIDTH) * 0.5);
        center_y = static_cast<double>(GAME_VOXEL_CHUNK_HEIGHT) * 0.5;
        center_z = static_cast<double>(world_chunk.world_z)
            + (static_cast<double>(GAME_VOXEL_CHUNK_DEPTH) * 0.5);
        radius = 140.0;
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
    if (pitched_z > 170.0 + radius)
        return (false);
    aspect_ratio = cache.half_width / cache.half_height;
    horizontal_limit = (pitched_z * std::tan(40.0 * 0.017453292519943295)
        * aspect_ratio) + radius;
    vertical_limit = (pitched_z * std::tan(40.0 * 0.017453292519943295))
        + radius;
    if (std::fabs(view_x) > horizontal_limit
        || std::fabs(pitched_y) > vertical_limit)
        return (false);
    return (true);
}

static void ft_vox_draw_mesh(ft_render_framebuffer &framebuffer,
    std::vector<double> &depth_buffer, const ft_vox_camera &camera,
    const ft_vox_render_cache &render_cache,
    const ft_vox_world_chunk &world_chunk)
{
    size_t index;
    chunk_mesh_vertex source_vertices[3];
    ft_vox_screen_vertex projected_vertices[3];

    if (ft_vox_chunk_is_near_camera(camera, world_chunk, render_cache)
        == false)
        return ;
    index = 0U;
    while (index + 2U < world_chunk.mesh.indices.size())
    {
        source_vertices[0] = world_chunk.mesh.vertices[
            world_chunk.mesh.indices[index]];
        source_vertices[1] = world_chunk.mesh.vertices[
            world_chunk.mesh.indices[index + 1U]];
        source_vertices[2] = world_chunk.mesh.vertices[
            world_chunk.mesh.indices[index + 2U]];
        if (ft_vox_triangle_faces_camera(camera, world_chunk,
                source_vertices) == FT_FALSE)
        {
            index = index + 3U;
            continue ;
        }
        if (ft_vox_project_vertex(camera, source_vertices[0],
                world_chunk.world_x, world_chunk.world_z, render_cache,
                &projected_vertices[0]) == true
            && ft_vox_project_vertex(camera, source_vertices[1],
                world_chunk.world_x, world_chunk.world_z, render_cache,
                &projected_vertices[1]) == true
            && ft_vox_project_vertex(camera, source_vertices[2],
                world_chunk.world_x, world_chunk.world_z, render_cache,
                &projected_vertices[2]) == true)
        {
            ft_vox_draw_triangle(framebuffer, depth_buffer, projected_vertices,
                source_vertices[0].block_id, source_vertices[0].face);
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

static const uint8_t *ft_vox_digit_bitmap(char character)
{
    static const uint8_t digits[10][5] = {
        { 7U, 5U, 5U, 5U, 7U },
        { 2U, 6U, 2U, 2U, 7U },
        { 7U, 1U, 7U, 4U, 7U },
        { 7U, 1U, 7U, 1U, 7U },
        { 5U, 5U, 7U, 1U, 1U },
        { 7U, 4U, 7U, 1U, 7U },
        { 7U, 4U, 7U, 5U, 7U },
        { 7U, 1U, 1U, 1U, 1U },
        { 7U, 5U, 7U, 5U, 7U },
        { 7U, 5U, 7U, 1U, 7U }
    };
    static const uint8_t letter_f[5] = { 7U, 4U, 6U, 4U, 4U };
    static const uint8_t letter_p[5] = { 6U, 5U, 6U, 4U, 4U };
    static const uint8_t letter_s[5] = { 7U, 4U, 7U, 1U, 7U };

    if (character >= '0' && character <= '9')
        return (digits[character - '0']);
    if (character == 'F')
        return (letter_f);
    if (character == 'P')
        return (letter_p);
    if (character == 'S')
        return (letter_s);
    return (nullptr);
}

static void ft_vox_draw_debug_char(ft_render_framebuffer &framebuffer,
    int32_t origin_x, int32_t origin_y, char character)
{
    const uint8_t *bitmap;
    int32_t row;
    int32_t column;
    int32_t pixel_x;
    int32_t pixel_y;

    bitmap = ft_vox_digit_bitmap(character);
    if (bitmap == nullptr)
        return ;
    row = 0;
    while (row < 5)
    {
        column = 0;
        while (column < 3)
        {
            if ((bitmap[row] & (1U << (2 - column))) != 0)
            {
                pixel_x = origin_x + column;
                pixel_y = origin_y + row;
                if (pixel_x >= 0 && pixel_x < framebuffer.width
                    && pixel_y >= 0 && pixel_y < framebuffer.height)
                {
                    framebuffer.pixels[(pixel_y * framebuffer.width)
                        + pixel_x] = ft_vox_rgba(245U, 245U, 235U);
                }
            }
            column = column + 1;
        }
        row = row + 1;
    }
}

static void ft_vox_draw_fps_overlay(ft_render_framebuffer &framebuffer,
    double display_fps)
{
    char text[16];
    int32_t index;
    int32_t x;

    if (display_fps < 0.0)
        return ;
    std::snprintf(text, sizeof(text), "FPS%03d",
        static_cast<int32_t>(display_fps + 0.5));
    index = 0;
    x = 8;
    while (text[index] != '\0')
    {
        ft_vox_draw_debug_char(framebuffer, x, 8, text[index]);
        x = x + 5;
        index = index + 1;
    }
}

static void ft_vox_blit_scaled(ft_render_framebuffer &destination,
    const ft_render_framebuffer &source)
{
    int32_t x;
    int32_t y;
    int32_t source_x;
    int32_t source_y;

    y = 0;
    while (y < destination.height)
    {
        source_y = (y * source.height) / destination.height;
        x = 0;
        while (x < destination.width)
        {
            source_x = (x * source.width) / destination.width;
            destination.pixels[(y * destination.width) + x]
                = source.pixels[(source_y * source.width) + source_x];
            x = x + 1;
        }
        y = y + 1;
    }
}

void ft_vox_render_world(ft_render_framebuffer &framebuffer,
    const ft_vox_camera &camera, const ft_vox_world &world,
    double display_fps)
{
    static std::vector<uint32_t> color_buffer;
    static std::vector<double> depth_buffer;
    ft_render_framebuffer render_target;
    ft_vox_render_cache render_cache;
    size_t required_size;
    int32_t chunk_index;

    if (framebuffer.pixels == nullptr || framebuffer.width <= 0
        || framebuffer.height <= 0)
        return ;
    render_target.width = framebuffer.width / 2;
    render_target.height = framebuffer.height / 2;
    if (render_target.width < 320)
        render_target.width = framebuffer.width;
    if (render_target.height < 180)
        render_target.height = framebuffer.height;
    required_size = static_cast<size_t>(render_target.width)
        * static_cast<size_t>(render_target.height);
    if (color_buffer.size() != required_size)
        color_buffer.resize(required_size);
    if (depth_buffer.size() != required_size)
        depth_buffer.resize(required_size);
    render_target.pixels = color_buffer.data();
    render_cache.yaw_cos = std::cos(camera.yaw);
    render_cache.yaw_sin = std::sin(camera.yaw);
    render_cache.pitch_cos = std::cos(camera.pitch);
    render_cache.pitch_sin = std::sin(camera.pitch);
    render_cache.focal_length = (static_cast<double>(render_target.width) * 0.5)
        / std::tan(40.0 * 0.017453292519943295);
    render_cache.half_width = static_cast<double>(render_target.width) * 0.5;
    render_cache.half_height = static_cast<double>(render_target.height) * 0.52;
    ft_vox_clear_frame(render_target, depth_buffer);
    chunk_index = 0;
    while (chunk_index < world.chunk_count)
    {
        ft_vox_draw_mesh(render_target, depth_buffer, camera, render_cache,
            world.chunks[chunk_index]);
        chunk_index = chunk_index + 1;
    }
    ft_vox_blit_scaled(framebuffer, render_target);
    ft_vox_draw_fps_overlay(framebuffer, display_fps);
    ft_vox_draw_crosshair(framebuffer);
}
