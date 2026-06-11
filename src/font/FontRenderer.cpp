#include "../../src/font/FontRenderer.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <ft2build.h>
#include FT_FREETYPE_H

struct FRImpl
{
    FT_Library library;
    FT_Face face;
    FRImpl() : library(nullptr), face(nullptr)
    {
    }
};

// clang-format off
const char *FontRenderer::COMIC_CANDIDATES[] = {
#if defined(__APPLE__)
    "/System/Library/Fonts/Supplemental/Comic Sans MS.ttf",
    "/Library/Fonts/Comic Sans MS.ttf",
    "/System/Library/Fonts/Supplemental/Arial.ttf",
    "/System/Library/Fonts/Helvetica.ttc",
#elif defined(_WIN32)
    "C:\\Windows\\Fonts\\comic.ttf",
    "C:\\Windows\\Fonts\\comicbd.ttf",
    "C:\\Windows\\Fonts\\arial.ttf",
#else
    "/usr/share/fonts/truetype/msttcorefonts/Comic_Sans_MS.ttf",
    "/usr/share/fonts/truetype/msttcorefonts/comic.ttf",
    "/usr/share/fonts/ms/Comic_Sans_MS.ttf",
    "/usr/share/fonts/corefonts/comic.ttf",
    "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
#endif
    nullptr
};
// clang-format on

const char *FontRenderer::find_comic_sans()
{
    for (int i = 0; COMIC_CANDIDATES[i]; ++i)
    {
        FILE *f = std::fopen(COMIC_CANDIDATES[i], "rb");
        if (f)
        {
            std::fclose(f);
            return COMIC_CANDIDATES[i];
        }
    }
    return nullptr;
}

FontRenderer::FontRenderer() : impl_(nullptr), pixel_height_(0.0f), loaded_(false)
{
}

FontRenderer::FontRenderer(const FontRenderer &other)
    : impl_(nullptr), pixel_height_(0.0f), loaded_(false)
{
    (void)other;
}

FontRenderer::~FontRenderer()
{
    destroy();
}

FontRenderer &FontRenderer::operator=(const FontRenderer &other)
{
    (void)other;
    return *this;
}

bool FontRenderer::load(float pixel_height)
{
    destroy();
    const char *path = find_comic_sans();
    if (!path)
        return false;
    FRImpl *im = new FRImpl();
    if (FT_Init_FreeType(&im->library) != 0)
    {
        delete im;
        return false;
    }
    if (FT_New_Face(im->library, path, 0, &im->face) != 0)
    {
        FT_Done_FreeType(im->library);
        delete im;
        return false;
    }
    if (FT_Set_Pixel_Sizes(im->face, 0, static_cast<FT_UInt>(pixel_height)) != 0)
    {
        FT_Done_Face(im->face);
        FT_Done_FreeType(im->library);
        delete im;
        return false;
    }
    impl_ = im;
    pixel_height_ = pixel_height;
    loaded_ = true;
    return true;
}

void FontRenderer::destroy()
{
    if (impl_)
    {
        FRImpl *im = static_cast<FRImpl *>(impl_);
        if (im->face)
            FT_Done_Face(im->face);
        if (im->library)
            FT_Done_FreeType(im->library);
        delete im;
        impl_ = nullptr;
    }
    loaded_ = false;
}

bool FontRenderer::is_loaded() const
{
    return loaded_;
}

int FontRenderer::line_height() const
{
    if (!loaded_)
        return static_cast<int>(pixel_height_) + 4;
    FRImpl *im = static_cast<FRImpl *>(impl_);
    return static_cast<int>(std::ceil(static_cast<double>(im->face->size->metrics.height) / 64.0));
}

int FontRenderer::text_width(const char *text) const
{
    if (!loaded_ || !text)
        return 0;
    FRImpl *im = static_cast<FRImpl *>(impl_);
    int w = 0;
    for (int i = 0; text[i]; ++i)
    {
        if (FT_Load_Char(im->face, static_cast<FT_ULong>(text[i]), FT_LOAD_DEFAULT) != 0)
            continue;
        w += static_cast<int>(im->face->glyph->advance.x >> 6);
    }
    return w;
}

void FontRenderer::blend_glyph(uint32_t *pixels, int buf_w, int buf_h, int draw_x, int draw_y,
                               uint8_t r, uint8_t g, uint8_t b) const
{
    FRImpl *im = static_cast<FRImpl *>(impl_);
    FT_Bitmap &bm = im->face->glyph->bitmap;
    for (int gy = 0; gy < static_cast<int>(bm.rows); ++gy)
    {
        for (int gx = 0; gx < static_cast<int>(bm.width); ++gx)
        {
            int px = draw_x + gx;
            int py = draw_y + gy;
            if (px < 0 || px >= buf_w || py < 0 || py >= buf_h)
                continue;
            uint8_t alpha = bm.buffer[gy * bm.pitch + gx];
            if (alpha == 0)
                continue;
            uint32_t &dst = pixels[py * buf_w + px];
            uint8_t dr = static_cast<uint8_t>((dst >> 16) & 0xFF);
            uint8_t dg = static_cast<uint8_t>((dst >> 8) & 0xFF);
            uint8_t db = static_cast<uint8_t>(dst & 0xFF);
            uint8_t nr = static_cast<uint8_t>((r * alpha + dr * (255 - alpha)) / 255);
            uint8_t ng = static_cast<uint8_t>((g * alpha + dg * (255 - alpha)) / 255);
            uint8_t nb = static_cast<uint8_t>((b * alpha + db * (255 - alpha)) / 255);
            dst = (static_cast<uint32_t>(nr) << 16) | (static_cast<uint32_t>(ng) << 8) |
                  static_cast<uint32_t>(nb);
        }
    }
}

void FontRenderer::draw_text(uint32_t *pixels, int buf_w, int buf_h, int x, int y, const char *text,
                             uint32_t colour) const
{
    if (!loaded_ || !pixels || !text)
        return;
    FRImpl *im = static_cast<FRImpl *>(impl_);
    uint8_t r = static_cast<uint8_t>((colour >> 16) & 0xFF);
    uint8_t g = static_cast<uint8_t>((colour >> 8) & 0xFF);
    uint8_t b = static_cast<uint8_t>(colour & 0xFF);
    int baseline =
        static_cast<int>(std::ceil(static_cast<double>(im->face->size->metrics.ascender) / 64.0));
    int cx = x;
    for (int i = 0; text[i]; ++i)
    {
        if (FT_Load_Char(im->face, static_cast<FT_ULong>(text[i]), FT_LOAD_RENDER) != 0)
            continue;
        FT_GlyphSlot slot = im->face->glyph;
        blend_glyph(pixels, buf_w, buf_h, cx + slot->bitmap_left, y + baseline - slot->bitmap_top,
                    r, g, b);
        cx += static_cast<int>(slot->advance.x >> 6);
    }
}

void FontRenderer::draw_text_centered(uint32_t *pixels, int buf_w, int buf_h, int cx, int y,
                                      const char *text, uint32_t colour) const
{
    draw_text(pixels, buf_w, buf_h, cx - text_width(text) / 2, y, text, colour);
}

FontRenderer &FontRenderer::instance()
{
    static FontRenderer fr;
    return fr;
}
