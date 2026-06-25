#include "../../src/font/FontRenderer.hpp"

struct BitmapGlyph
{
    char character;
    const char *rows[7];
};

// clang-format off
static const BitmapGlyph GLYPHS[] = {
    {'A', {" ### ", "#   #", "#   #", "#####", "#   #", "#   #", "#   #"}},
    {'B', {"#### ", "#   #", "#   #", "#### ", "#   #", "#   #", "#### "}},
    {'C', {" ####", "#    ", "#    ", "#    ", "#    ", "#    ", " ####"}},
    {'D', {"#### ", "#   #", "#   #", "#   #", "#   #", "#   #", "#### "}},
    {'E', {"#####", "#    ", "#    ", "#####", "#    ", "#    ", "#####"}},
    {'F', {"#####", "#    ", "#    ", "#####", "#    ", "#    ", "#    "}},
    {'G', {" ####", "#    ", "#    ", "# ###", "#   #", "#   #", " ####"}},
    {'H', {"#   #", "#   #", "#   #", "#####", "#   #", "#   #", "#   #"}},
    {'I', {"#####", "  #  ", "  #  ", "  #  ", "  #  ", "  #  ", "#####"}},
    {'J', {"#####", "   # ", "   # ", "   # ", "#  # ", "#  # ", " ##  "}},
    {'K', {"#   #", "#  # ", "# #  ", "##   ", "# #  ", "#  # ", "#   #"}},
    {'L', {"#    ", "#    ", "#    ", "#    ", "#    ", "#    ", "#####"}},
    {'M', {"#   #", "## ##", "# # #", "#   #", "#   #", "#   #", "#   #"}},
    {'N', {"#   #", "##  #", "# # #", "#  ##", "#   #", "#   #", "#   #"}},
    {'O', {" ### ", "#   #", "#   #", "#   #", "#   #", "#   #", " ### "}},
    {'P', {"#### ", "#   #", "#   #", "#### ", "#    ", "#    ", "#    "}},
    {'Q', {" ### ", "#   #", "#   #", "#   #", "# # #", "#  # ", " ## #"}},
    {'R', {"#### ", "#   #", "#   #", "#### ", "# #  ", "#  # ", "#   #"}},
    {'S', {" ####", "#    ", "#    ", " ### ", "    #", "    #", "#### "}},
    {'T', {"#####", "  #  ", "  #  ", "  #  ", "  #  ", "  #  ", "  #  "}},
    {'U', {"#   #", "#   #", "#   #", "#   #", "#   #", "#   #", " ### "}},
    {'V', {"#   #", "#   #", "#   #", "#   #", " # # ", " # # ", "  #  "}},
    {'W', {"#   #", "#   #", "#   #", "# # #", "# # #", "## ##", "#   #"}},
    {'X', {"#   #", " # # ", "  #  ", "  #  ", "  #  ", " # # ", "#   #"}},
    {'Y', {"#   #", " # # ", "  #  ", "  #  ", "  #  ", "  #  ", "  #  "}},
    {'Z', {"#####", "    #", "   # ", "  #  ", " #   ", "#    ", "#####"}},
    {'0', {" ### ", "#   #", "#  ##", "# # #", "##  #", "#   #", " ### "}},
    {'1', {"  #  ", " ##  ", "  #  ", "  #  ", "  #  ", "  #  ", "#####"}},
    {'2', {" ### ", "#   #", "    #", "   # ", "  #  ", " #   ", "#####"}},
    {'3', {"#####", "    #", "   # ", "  ## ", "    #", "#   #", " ### "}},
    {'4', {"#   #", "#   #", "#   #", "#####", "    #", "    #", "    #"}},
    {'5', {"#####", "#    ", "#    ", "#### ", "    #", "    #", "#### "}},
    {'6', {" ### ", "#    ", "#    ", "#### ", "#   #", "#   #", " ### "}},
    {'7', {"#####", "    #", "   # ", "  #  ", " #   ", " #   ", " #   "}},
    {'8', {" ### ", "#   #", "#   #", " ### ", "#   #", "#   #", " ### "}},
    {'9', {" ### ", "#   #", "#   #", " ####", "    #", "    #", " ### "}},
    {' ', {"     ", "     ", "     ", "     ", "     ", "     ", "     "}},
    {'_', {"     ", "     ", "     ", "     ", "     ", "     ", "#####"}},
    {'-', {"     ", "     ", "     ", "#####", "     ", "     ", "     "}},
    {'.', {"     ", "     ", "     ", "     ", "     ", "  ## ", "  ## "}},
    {':', {"     ", "  ## ", "  ## ", "     ", "  ## ", "  ## ", "     "}},
    {'/', {"    #", "   # ", "   # ", "  #  ", " #   ", " #   ", "#    "}},
    {'%', {"##  #", "## # ", "  #  ", " # ##", "#  ##", "     ", "     "}}
};
// clang-format on

static const BitmapGlyph *find_glyph(char character)
{
    size_t index;

    if (character >= 'a' && character <= 'z')
        character = static_cast<char>(character - 'a' + 'A');
    index = 0U;
    while (index < sizeof(GLYPHS) / sizeof(GLYPHS[0]))
    {
        if (GLYPHS[index].character == character)
            return (&GLYPHS[index]);
        ++index;
    }
    return (ft_nullptr);
}

static void draw_glyph(uint32_t *pixels, int buf_w, int buf_h, int origin_x, int origin_y,
    const BitmapGlyph *glyph, int scale, uint32_t colour)
{
    int row_index;
    int col_index;
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    if (glyph == ft_nullptr || pixels == ft_nullptr || scale <= 0)
        return ;
    red = static_cast<uint8_t>((colour >> 16) & 0xFFU);
    green = static_cast<uint8_t>((colour >> 8) & 0xFFU);
    blue = static_cast<uint8_t>(colour & 0xFFU);
    row_index = 0;
    while (row_index < 7)
    {
        col_index = 0;
        while (col_index < 5)
        {
            if (glyph->rows[row_index][col_index] != ' ')
            {
                int sy = 0;

                while (sy < scale)
                {
                    int sx = 0;

                    while (sx < scale)
                    {
                        int px = origin_x + col_index * scale + sx;
                        int py = origin_y + row_index * scale + sy;

                        if (px >= 0 && px < buf_w && py >= 0 && py < buf_h)
                        {
                            pixels[py * buf_w + px] = (static_cast<uint32_t>(red) << 16)
                                | (static_cast<uint32_t>(green) << 8)
                                | static_cast<uint32_t>(blue);
                        }
                        ++sx;
                    }
                    ++sy;
                }
            }
            ++col_index;
        }
        ++row_index;
    }
}

FontRenderer::FontRenderer() : pixel_height_(0.0f), loaded_(false)
{
    return ;
}

FontRenderer::FontRenderer(const FontRenderer &other)
    : pixel_height_(0.0f), loaded_(false)
{
    (void)other;
    return ;
}

FontRenderer::~FontRenderer()
{
    destroy();
    return ;
}

FontRenderer &FontRenderer::operator=(const FontRenderer &other)
{
    (void)other;
    return (*this);
}

bool FontRenderer::load(float pixel_height)
{
    destroy();
    pixel_height_ = pixel_height;
    loaded_ = true;
    return (true);
}

void FontRenderer::destroy()
{
    loaded_ = false;
    return ;
}

bool FontRenderer::is_loaded() const
{
    return (loaded_);
}

int FontRenderer::line_height() const
{
    if (!loaded_)
        return (0);
    return (static_cast<int>(std::ceil(static_cast<double>(pixel_height_))));
}

int FontRenderer::text_width(const char *text) const
{
    int glyph_scale;

    if (!loaded_ || text == ft_nullptr)
        return (0);
    glyph_scale = std::max(1, static_cast<int>(std::lround(pixel_height_ / 7.0f)));
    return (static_cast<int>(std::strlen(text)) * glyph_scale * 6);
}

void FontRenderer::blend_glyph(uint32_t *pixels, int buf_w, int buf_h, int draw_x, int draw_y,
    uint8_t r, uint8_t g, uint8_t b) const
{
    (void)pixels;
    (void)buf_w;
    (void)buf_h;
    (void)draw_x;
    (void)draw_y;
    (void)r;
    (void)g;
    (void)b;
    return ;
}

void FontRenderer::draw_text(uint32_t *pixels, int buf_w, int buf_h, int x, int y, const char *text,
    uint32_t colour) const
{
    int glyph_scale;
    int cursor_x;
    int index;

    if (!loaded_ || pixels == ft_nullptr || text == ft_nullptr)
        return ;
    glyph_scale = std::max(1, static_cast<int>(std::lround(pixel_height_ / 7.0f)));
    cursor_x = x;
    index = 0;
    while (text[index] != '\0')
    {
        const BitmapGlyph *glyph;

        glyph = find_glyph(text[index]);
        if (glyph != ft_nullptr)
            draw_glyph(pixels, buf_w, buf_h, cursor_x, y, glyph, glyph_scale, colour);
        cursor_x += glyph_scale * 6;
        ++index;
    }
    return ;
}

void FontRenderer::draw_text_centered(uint32_t *pixels, int buf_w, int buf_h, int cx, int y,
    const char *text, uint32_t colour) const
{
    int glyph_scale;
    int width;

    if (!loaded_ || text == ft_nullptr)
        return ;
    glyph_scale = std::max(1, static_cast<int>(std::lround(pixel_height_ / 7.0f)));
    width = static_cast<int>(std::strlen(text)) * glyph_scale * 6;
    draw_text(pixels, buf_w, buf_h, cx - width / 2, y, text, colour);
    return ;
}

FontRenderer &FontRenderer::instance()
{
    static FontRenderer fr;
    return (fr);
}
