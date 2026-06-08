#ifndef GPUR_RENDERER_HPP
# define GPUR_RENDERER_HPP

#include "../../src/gpur/gl_funcs.hpp"
#include "../../src/gpur/GpuShader.hpp"
#include "../../src/gpur/GpuTextureAtlas.hpp"
#include "../../src/camera/Camera.hpp"
#include "../../src/world/World.hpp"
#include "../../src/coordinates/WorldCoordinates.hpp"
#include <cstdint>
#include <vector>

struct MegaVertex
{
    float    x, y, z;
    uint16_t tu, tv;
    uint32_t block_id;
    uint32_t face;
};

class GpuRenderer
{
    GpuShader       _world_shader;
    GpuShader       _sky_shader;
    GpuShader       _overlay_shader;
    GpuShader       _overlay_tex_shader;
    GpuTextureAtlas _atlas;

    GLuint _mega_vao;
    GLuint _mega_vbo;
    GLuint _mega_ebo;
    size_t _mega_vbo_cap;
    size_t _mega_ebo_cap;
    std::vector<MegaVertex> _mega_verts;
    std::vector<uint32_t>   _mega_idxs;

    GLuint _sky_vao;
    GLuint _crosshair_vbo;
    GLuint _crosshair_vao;
    GLuint _overlay_tex;
    int    _overlay_w;
    int    _overlay_h;
    std::vector<uint8_t> _overlay_rgba;
    bool   _ready;
    int    _width;
    int    _height;

    float _tile_uvs[96 * 4];
    float _fallback_colors[16 * 3];

    GLint _u_mvp;
    GLint _u_atlas;
    GLint _u_atlas_loaded;
    GLint _u_tile_uvs;
    GLint _u_fallback;
    GLint _u_sky_size;
    GLint _u_overlay_color;
    GLint _u_overlay_sampler;
    GLint _u_overlay_ndc_br;

    void build_mvp(float out[16], const Camera &camera,
        int width, int height, float near_z, float far_z) const noexcept;
    void draw_sky() const noexcept;
    void draw_crosshair() const noexcept;

    public:
        GpuRenderer() noexcept;
        ~GpuRenderer() noexcept { destroy(); }

        GpuRenderer(const GpuRenderer &)            = delete;
        GpuRenderer &operator=(const GpuRenderer &) = delete;

        bool initialize(int width, int height) noexcept;
        void destroy() noexcept;
        void resize(int width, int height) noexcept;
        void render(const Camera &camera, const World &world) noexcept;
        void upload_overlay(const uint32_t *pixels, int w, int h) noexcept;
        void draw_overlay() noexcept;
        bool is_ready() const noexcept { return _ready; }
};

#endif
