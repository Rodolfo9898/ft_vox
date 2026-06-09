#ifndef GPUR_RENDERER_HPP
# define GPUR_RENDERER_HPP

#include "../../Libft/Modules/GPGR/gl_funcs.hpp"
#include "../../src/gpur/GpuMvpBuilder.hpp"
#include "../../src/gpur/GpuSkyRenderer.hpp"
#include "../../src/gpur/GpuWorldRenderer.hpp"
#include "../../src/gpur/GpuHudRenderer.hpp"
#include "../../src/camera/Camera.hpp"
#include "../../src/world/World.hpp"
#include <cstdint>

class GpuRenderer
{
    GpuMvpBuilder    _mvp;
    GpuSkyRenderer   _sky;
    GpuWorldRenderer _world;
    GpuHudRenderer   _hud;

    bool _ready;
    int  _width;
    int  _height;

    public:
        GpuRenderer() noexcept;
        GpuRenderer(const GpuRenderer &other);
        ~GpuRenderer() noexcept;
        GpuRenderer &operator=(const GpuRenderer &other);

        bool initialize(int width, int height) noexcept;
        void destroy() noexcept;
        void resize(int width, int height) noexcept;
        void render(const Camera &camera, const World &world) noexcept;

        void upload_overlay(const uint32_t *pixels, int w, int h) noexcept;
        void draw_overlay() noexcept;
        void draw_tint(float r, float g, float b, float a) noexcept;
        void clear_screen() noexcept;
        void upload_menu(const uint32_t *pixels, int w, int h) noexcept;
        void draw_menu(float alpha) noexcept;

        bool     is_ready() const noexcept;
        uint32_t gpu_mb_approx() const noexcept;
};

#endif
