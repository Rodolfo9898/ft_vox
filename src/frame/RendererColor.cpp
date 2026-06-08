#include "../../src/frame/RendererColor.hpp"
#ifndef GAME_USE_VOXEL_REGION_BACKEND
# define GAME_USE_VOXEL_REGION_BACKEND
#endif
#include "../../Libft/Modules/Voxel/voxel.hpp"

RendererColor::RendererColor()
{
}

RendererColor::RendererColor(const RendererColor &other)
{
    *this = other;
}

RendererColor::~RendererColor()
{
}

RendererColor &RendererColor::operator=(const RendererColor &other)
{
    (void)other;
    return (*this);
}

uint32_t RendererColor::rgba(uint8_t red, uint8_t green, uint8_t blue)
{
    return ((static_cast<uint32_t>(red) << 16U)
        | (static_cast<uint32_t>(green) << 8U)
        | static_cast<uint32_t>(blue));
}

uint8_t RendererColor::shade_channel(uint8_t channel, double shade)
{
    int32_t value;

    value = static_cast<int32_t>(static_cast<double>(channel) * shade);
    if (value < 0)
        value = 0;
    if (value > 255)
        value = 255;
    return (static_cast<uint8_t>(value));
}

uint32_t RendererColor::block_color(uint32_t block_id, uint8_t face)
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    double  shade;

    red = 82U;
    green = 72U;
    blue = 105U;
    if (block_id == TERRAIN_GENERATOR_GRASS_BLOCK)
    {
        red = 91U;
        green = 184U;
        blue = 141U;
    }
    else if (block_id == TERRAIN_GENERATOR_DIRT_BLOCK)
    {
        red = 169U;
        green = 91U;
        blue = 96U;
    }
    else if (block_id == TERRAIN_GENERATOR_STONE_BLOCK)
    {
        red = 72U;
        green = 91U;
        blue = 122U;
    }
    else if (block_id == TERRAIN_GENERATOR_SHRUB_BLOCK)
    {
        red = 190U;
        green = 151U;
        blue = 74U;
    }
    else if (block_id == TERRAIN_GENERATOR_OAK_LOG_BLOCK)
    {
        red = 151U;
        green = 86U;
        blue = 98U;
    }
    else if (block_id == TERRAIN_GENERATOR_OAK_LEAVES_BLOCK)
    {
        red = 66U;
        green = 151U;
        blue = 132U;
    }
    else if (block_id == TERRAIN_GENERATOR_CACTUS_BLOCK)
    {
        red = 67U;
        green = 151U;
        blue = 135U;
    }
    else if (block_id == TERRAIN_GENERATOR_WATER_BLOCK)
    {
        red = 30U;
        green = 100U;
        blue = 200U;
    }
    shade = 0.72;
    if (face == CHUNK_MESH_FACE_UP)
        shade = 1.0;
    else if (face == CHUNK_MESH_FACE_NORTH || face == CHUNK_MESH_FACE_WEST)
        shade = 0.58;
    else if (face == CHUNK_MESH_FACE_EAST)
        shade = 0.82;
    return (rgba(shade_channel(red, shade),
        shade_channel(green, shade), shade_channel(blue, shade)));
}

uint32_t RendererColor::shade_color(uint32_t color, double shade)
{
    return (rgba(
        shade_channel(static_cast<uint8_t>((color >> 16U) & 0xFFU),
            shade),
        shade_channel(static_cast<uint8_t>((color >> 8U) & 0xFFU),
            shade),
        shade_channel(static_cast<uint8_t>(color & 0xFFU), shade)));
}

double RendererColor::face_shade(uint8_t face)
{
    if (face == CHUNK_MESH_FACE_UP)
        return (1.0);
    if (face == CHUNK_MESH_FACE_NORTH || face == CHUNK_MESH_FACE_WEST)
        return (0.58);
    if (face == CHUNK_MESH_FACE_EAST)
        return (0.82);
    return (0.72);
}
