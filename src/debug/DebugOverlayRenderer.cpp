#include "../../src/debug/DebugOverlayRenderer.hpp"

DebugOverlayRenderer::DebugOverlayRenderer()
{
}

DebugOverlayRenderer::DebugOverlayRenderer(const DebugOverlayRenderer &other)
{
    *this = other;
}

DebugOverlayRenderer::~DebugOverlayRenderer()
{
}

DebugOverlayRenderer &DebugOverlayRenderer::operator=(const DebugOverlayRenderer &other)
{
    (void)other;
    return (*this);
}

void DebugOverlayRenderer::draw_crosshair(ft_render_framebuffer &framebuffer)
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
            framebuffer.pixels[pixel_index] = RendererColor::rgba(245U, 245U, 235U);
        }
        if (center_y + offset >= 0 && center_y + offset < framebuffer.height)
        {
            pixel_index = static_cast<size_t>(((center_y + offset)
                * framebuffer.width) + center_x);
            framebuffer.pixels[pixel_index] = RendererColor::rgba(245U, 245U, 235U);
        }
        offset = offset + 1;
    }
}

const uint8_t *DebugOverlayRenderer::digit_bitmap(char character)
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
    static const uint8_t letter_m[5] = { 5U, 7U, 7U, 5U, 5U };
    static const uint8_t letter_c[5] = { 7U, 4U, 4U, 4U, 7U };
    static const uint8_t letter_l[5] = { 4U, 4U, 4U, 4U, 7U };
    static const uint8_t letter_r[5] = { 6U, 5U, 6U, 5U, 5U };
    static const uint8_t letter_d[5] = { 6U, 5U, 5U, 5U, 6U };
    static const uint8_t letter_b[5] = { 6U, 5U, 6U, 5U, 6U };

    if (character >= '0' && character <= '9')
        return (digits[character - '0']);
    if (character == 'F')
        return (letter_f);
    if (character == 'P')
        return (letter_p);
    if (character == 'S')
        return (letter_s);
    if (character == 'M')
        return (letter_m);
    if (character == 'C')
        return (letter_c);
    if (character == 'L')
        return (letter_l);
    if (character == 'R')
        return (letter_r);
    if (character == 'D')
        return (letter_d);
    if (character == 'B')
        return (letter_b);
    return (nullptr);
}

void DebugOverlayRenderer::draw_debug_char(ft_render_framebuffer &framebuffer,
    int32_t origin_x, int32_t origin_y, char character, int32_t scale)
{
    const uint8_t *bitmap;
    int32_t row;
    int32_t column;
    int32_t scale_x;
    int32_t scale_y;
    int32_t pixel_x;
    int32_t pixel_y;

    bitmap = digit_bitmap(character);
    if (bitmap == nullptr || scale <= 0)
        return ;
    row = 0;
    while (row < 5)
    {
        column = 0;
        while (column < 3)
        {
            if ((bitmap[row] & (1U << (2 - column))) != 0)
            {
                scale_y = 0;
                while (scale_y < scale)
                {
                    scale_x = 0;
                    while (scale_x < scale)
                    {
                        pixel_x = origin_x + (column * scale) + scale_x;
                        pixel_y = origin_y + (row * scale) + scale_y;
                        if (pixel_x >= 0 && pixel_x < framebuffer.width
                            && pixel_y >= 0 && pixel_y < framebuffer.height)
                        {
                            framebuffer.pixels[(pixel_y * framebuffer.width)
                                + pixel_x] = RendererColor::rgba(245U, 245U, 235U);
                        }
                        scale_x = scale_x + 1;
                    }
                    scale_y = scale_y + 1;
                }
            }
            column = column + 1;
        }
        row = row + 1;
    }
}

void DebugOverlayRenderer::draw_debug_text(ft_render_framebuffer &framebuffer,
    int32_t origin_x, int32_t origin_y, const char *text, int32_t scale)
{
    int32_t index;
    int32_t x;

    if (text == nullptr)
        return ;
    index = 0;
    x = origin_x;
    while (text[index] != '\0')
    {
        if (text[index] == ' ')
            x = x + (5 * scale);
        else
        {
            draw_debug_char(framebuffer, x, origin_y, text[index],
                scale);
            x = x + (5 * scale);
        }
        index = index + 1;
    }
}

void DebugOverlayRenderer::draw_debug_overlay(ft_render_framebuffer &framebuffer,
    const RenderDebug *debug)
{
    char text[64];
    int32_t scale;

    if (debug == nullptr)
        return ;
    scale = 3;
    if (framebuffer.width < 900 || framebuffer.height < 520)
        scale = 2;
    std::snprintf(text, sizeof(text), "FPS%03d MS%03d C%03d L%03d R%03d B%02u",
        static_cast<int32_t>(debug->fps + 0.5),
        static_cast<int32_t>(debug->frame_ms + 0.5),
        debug->visible_chunks, debug->loaded_chunks, debug->render_distance,
        debug->selected_block_id % 100U);
    draw_debug_text(framebuffer, 12, 12, text, scale);
}
