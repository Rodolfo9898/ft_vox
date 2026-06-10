#ifndef FONT_RENDERER_HPP
# define FONT_RENDERER_HPP

#include <cstdint>

class FontRenderer
{
    void  *impl_;
    float  pixel_height_;
    bool   loaded_;

    public:
        FontRenderer();
        FontRenderer(const FontRenderer &other);
        ~FontRenderer();
        FontRenderer &operator=(const FontRenderer &other);

        bool load(float pixel_height);
        void destroy();

        bool is_loaded()                const;
        int  line_height()              const;
        int  text_width(const char *text) const;

        void draw_text(uint32_t *pixels, int buf_w, int buf_h,
            int x, int y, const char *text, uint32_t colour) const;

        void draw_text_centered(uint32_t *pixels, int buf_w, int buf_h,
            int cx, int y, const char *text, uint32_t colour) const;

        static FontRenderer &instance();
};

#endif
