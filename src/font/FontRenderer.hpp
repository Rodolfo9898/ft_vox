#ifndef FONT_RENDERER_HPP
#define FONT_RENDERER_HPP


#include "../ft_vox.hpp"
class FontRenderer
{
  public:
    FontRenderer();
    FontRenderer(const FontRenderer &other);
    ~FontRenderer();
    FontRenderer &operator=(const FontRenderer &other);

    bool load(float pixel_height);
    void destroy();

    bool is_loaded() const;
    int line_height() const;
    int text_width(const char *text) const;

    void draw_text(uint32_t *pixels, int buf_w, int buf_h, int x, int y, const char *text,
                   uint32_t colour) const;
    void draw_text_centered(uint32_t *pixels, int buf_w, int buf_h, int cx, int y, const char *text,
                            uint32_t colour) const;

    static FontRenderer &instance();

  private:
    void *impl_;
    float pixel_height_;
    bool loaded_;

    static const char *find_comic_sans();
    static const char *COMIC_CANDIDATES[];

    void blend_glyph(uint32_t *pixels, int buf_w, int buf_h, int draw_x, int draw_y, uint8_t r,
                     uint8_t g, uint8_t b) const;
};

#endif
