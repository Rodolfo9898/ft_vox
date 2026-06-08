#include "../../src/assets/TextureAtlas.hpp"

TextureAtlas::TextureAtlas()
{
    this->width = 0;
    this->height = 0;
    this->tile_width = 0;
    this->tile_height = 0;
    this->loaded = false;
    this->attempted_load = false;
}

TextureAtlas::TextureAtlas(const TextureAtlas &other)
{
    *this = other;
}

TextureAtlas::~TextureAtlas()
{
}

TextureAtlas &TextureAtlas::operator=(const TextureAtlas &other)
{
    if (this != &other)
    {
        this->pixels = other.pixels;
        this->width = other.width;
        this->height = other.height;
        this->tile_width = other.tile_width;
        this->tile_height = other.tile_height;
        this->loaded = other.loaded;
        this->attempted_load = other.attempted_load;
    }
    return (*this);
}

uint16_t TextureAtlas::read_le_u16(const uint8_t *data)
{
    return (static_cast<uint16_t>(data[0])
        | static_cast<uint16_t>(static_cast<uint16_t>(data[1]) << 8U));
}

uint32_t TextureAtlas::read_le_u32(const uint8_t *data)
{
    return (static_cast<uint32_t>(data[0])
        | (static_cast<uint32_t>(data[1]) << 8U)
        | (static_cast<uint32_t>(data[2]) << 16U)
        | (static_cast<uint32_t>(data[3]) << 24U));
}

int32_t TextureAtlas::read_le_i32(const uint8_t *data)
{
    return (static_cast<int32_t>(read_le_u32(data)));
}

bool TextureAtlas::load_bmp_texture_atlas(TextureAtlas *atlas,
    const char *path)
{
    FILE *file;
    uint8_t header[54];
    uint32_t data_offset;
    int32_t width;
    int32_t height_signed;
    int32_t height;
    uint16_t bits_per_pixel;
    uint32_t compression;
    size_t row_stride;
    size_t y;
    size_t source_y;
    size_t x;
    long row_offset;
    uint8_t pixel[3];

    if (atlas == nullptr || path == nullptr)
        return (false);
    file = std::fopen(path, "rb");
    if (file == nullptr)
        return (false);
    if (std::fread(header, 1U, sizeof(header), file) != sizeof(header))
    {
        std::fclose(file);
        return (false);
    }
    if (header[0] != 'B' || header[1] != 'M')
    {
        std::fclose(file);
        return (false);
    }
    data_offset = read_le_u32(header + 10);
    width = read_le_i32(header + 18);
    height_signed = read_le_i32(header + 22);
    bits_per_pixel = read_le_u16(header + 28);
    compression = read_le_u32(header + 30);
    if (width <= 0 || height_signed == 0 || bits_per_pixel != 24U
        || compression != 0U)
    {
        std::fclose(file);
        return (false);
    }
    height = height_signed < 0 ? -height_signed : height_signed;
    atlas->pixels.assign(static_cast<size_t>(width) * static_cast<size_t>(height),
        0U);
    row_stride = ((static_cast<size_t>(width) * 3U) + 3U) & ~static_cast<size_t>(3U);
    y = 0U;
    while (y < static_cast<size_t>(height))
    {
        source_y = height_signed < 0 ? y : static_cast<size_t>(height - 1) - y;
        row_offset = static_cast<long>(data_offset + (source_y * row_stride));
        if (std::fseek(file, row_offset, SEEK_SET) != 0)
        {
            std::fclose(file);
            return (false);
        }
        x = 0U;
        while (x < static_cast<size_t>(width))
        {
            if (std::fread(pixel, 1U, sizeof(pixel), file) != sizeof(pixel))
            {
                std::fclose(file);
                return (false);
            }
            atlas->pixels[(y * static_cast<size_t>(width)) + x]
                = RendererColor::rgba(pixel[2], pixel[1], pixel[0]);
            x = x + 1U;
        }
        y = y + 1U;
    }
    std::fclose(file);
    atlas->width = width;
    atlas->height = height;
    atlas->tile_width = width / 4;
    atlas->tile_height = height / 4;
    atlas->loaded = atlas->tile_width > 0 && atlas->tile_height > 0;
    return (atlas->loaded);
}

const TextureAtlas &TextureAtlas::instance()
{
    static TextureAtlas atlas;

    if (atlas.attempted_load == false)
    {
        atlas.width = 0;
        atlas.height = 0;
        atlas.tile_width = 0;
        atlas.tile_height = 0;
        atlas.loaded = false;
        atlas.attempted_load = true;
        (void)load_bmp_texture_atlas(&atlas,
            "assets/textures/voxel_atlas.bmp");
    }
    return (atlas);
}

void TextureAtlas::atlas_tile_for_block(uint32_t block_id, uint8_t face,
    int32_t *tile_x, int32_t *tile_y)
{
    if (tile_x == nullptr || tile_y == nullptr)
        return ;
    *tile_x = 2;
    *tile_y = 2;
    if (block_id == TERRAIN_GENERATOR_GRASS_BLOCK)
    {
        *tile_x = (face == CHUNK_MESH_FACE_UP) ? 0 : 1;
        *tile_y = 0;
    }
    else if (block_id == TERRAIN_GENERATOR_DIRT_BLOCK)
    {
        *tile_x = 2;
        *tile_y = 0;
    }
    else if (block_id == TERRAIN_GENERATOR_STONE_BLOCK)
    {
        *tile_x = 3;
        *tile_y = 0;
    }
    else if (block_id == TERRAIN_GENERATOR_SHRUB_BLOCK)
    {
        *tile_x = 1;
        *tile_y = 2;
    }
    else if (block_id == TERRAIN_GENERATOR_OAK_LOG_BLOCK)
    {
        *tile_x = 2;
        *tile_y = 1;
    }
    else if (block_id == TERRAIN_GENERATOR_OAK_LEAVES_BLOCK)
    {
        *tile_x = 3;
        *tile_y = 1;
    }
    else if (block_id == TERRAIN_GENERATOR_CACTUS_BLOCK)
    {
        *tile_x = 0;
        *tile_y = 2;
    }
    else if (block_id == TERRAIN_GENERATOR_WATER_BLOCK)
    {
        *tile_x = 0;
        *tile_y = 1;
    }
}

double TextureAtlas::fraction(double value)
{
    return (value - std::floor(value));
}

TriangleTexture TextureAtlas::prepare_triangle_texture(
    uint32_t block_id, uint8_t face)
{
    const TextureAtlas &atlas = instance();
    TriangleTexture texture;
    int32_t tile_x;
    int32_t tile_y;
    int32_t margin_x;
    int32_t margin_y;
    static TriangleTexture cached_textures[16][6];
    static bool cached_texture_ready[16][6];

    if (block_id < 16U && face < 6U && cached_texture_ready[block_id][face] == true)
        return (cached_textures[block_id][face]);
    texture.atlas = &atlas;
    texture.fallback_color = RendererColor::block_color(block_id, face);
    texture.base_x = 0;
    texture.base_y = 0;
    texture.sample_width = 1;
    texture.sample_height = 1;
    texture.shade = RendererColor::face_shade(face);
    texture.loaded = false;
    if (atlas.is_loaded() == false)
        return (texture);
    atlas_tile_for_block(block_id, face, &tile_x, &tile_y);
    margin_x = std::max(2, atlas.get_tile_width() / 32);
    margin_y = std::max(2, atlas.get_tile_height() / 32);
    texture.base_x = (tile_x * atlas.get_tile_width()) + margin_x;
    texture.base_y = (tile_y * atlas.get_tile_height()) + margin_y;
    texture.sample_width = std::max(1, atlas.get_tile_width() - (margin_x * 2));
    texture.sample_height = std::max(1, atlas.get_tile_height() - (margin_y * 2));
    texture.loaded = true;
    if (block_id < 16U && face < 6U)
    {
        cached_textures[block_id][face] = texture;
        cached_texture_ready[block_id][face] = true;
    }
    return (texture);
}

uint32_t TextureAtlas::sample_triangle_texture(
    const TriangleTexture &texture, double texture_u, double texture_v)
{
    int32_t pixel_x;
    int32_t pixel_y;
    double u;
    double v;
    int32_t atlas_width;
    int32_t atlas_height;

    if (texture.loaded == false || texture.atlas == nullptr)
        return (texture.fallback_color);
    if (texture_u >= 0.0 && texture_u < 1.0 && texture_v >= 0.0
        && texture_v < 1.0)
    {
        pixel_x = texture.base_x
            + static_cast<int32_t>(texture_u
                * static_cast<double>(texture.sample_width));
        pixel_y = texture.base_y
            + static_cast<int32_t>(texture_v
                * static_cast<double>(texture.sample_height));
        return (texture.atlas->pixel_at(pixel_x, pixel_y));
    }
    u = texture_u - static_cast<double>(static_cast<int64_t>(texture_u));
    v = texture_v - static_cast<double>(static_cast<int64_t>(texture_v));
    if (u < 0.0)
        u += 1.0;
    if (v < 0.0)
        v += 1.0;
    pixel_x = texture.base_x
        + static_cast<int32_t>(u * static_cast<double>(texture.sample_width));
    pixel_y = texture.base_y
        + static_cast<int32_t>(v * static_cast<double>(texture.sample_height));
    atlas_width = texture.atlas->get_width();
    atlas_height = texture.atlas->get_height();
    if (pixel_x < 0)
        pixel_x = 0;
    else if (pixel_x >= atlas_width)
        pixel_x = atlas_width - 1;
    if (pixel_y < 0)
        pixel_y = 0;
    else if (pixel_y >= atlas_height)
        pixel_y = atlas_height - 1;
    return (texture.atlas->pixel_at(pixel_x, pixel_y));
}

int32_t TextureAtlas::get_width() const
{
    return (this->width);
}

int32_t TextureAtlas::get_height() const
{
    return (this->height);
}

int32_t TextureAtlas::get_tile_width() const
{
    return (this->tile_width);
}

int32_t TextureAtlas::get_tile_height() const
{
    return (this->tile_height);
}

bool TextureAtlas::is_loaded() const
{
    return (this->loaded);
}

uint32_t TextureAtlas::pixel_at(int32_t x, int32_t y) const
{
    return (this->pixels[(static_cast<size_t>(y)
        * static_cast<size_t>(this->width)) + static_cast<size_t>(x)]);
}
