#ifndef GPU_SKY_RENDERER_HPP
# define GPU_SKY_RENDERER_HPP

#include "../../Libft/Modules/GPGR/GpuShader.hpp"
#include "../../Libft/Modules/GPGR/gl_funcs.hpp"

class GpuSkyRenderer
{
    GpuShader _sky_shader;
    GpuShader _tint_shader;
    GLuint    _fullscreen_vao;
    GLint     _u_sky_size;
    GLint     _u_tint_color;

    public:
        GpuSkyRenderer();
        GpuSkyRenderer(const GpuSkyRenderer &other);
        ~GpuSkyRenderer();
        GpuSkyRenderer &operator=(const GpuSkyRenderer &other);

        bool initialize() noexcept;
        void destroy() noexcept;

        void draw_sky(int width, int height) const noexcept;
        void draw_tint(float r, float g, float b, float a) const noexcept;
        void draw_fullscreen() const noexcept;
};

#endif
