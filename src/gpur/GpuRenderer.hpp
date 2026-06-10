#ifndef GPUR_RENDERER_HPP
# define GPUR_RENDERER_HPP

#ifndef GAME_USE_VOXEL_REGION_BACKEND
# define GAME_USE_VOXEL_REGION_BACKEND
#endif
#include <cstring>
#include <cmath>
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include "../../src/meshes/MeshCuller.hpp"
#include "../../src/frame/RenderCache.hpp"
#include "../../src/assets/TextureAtlas.hpp"
#include "../../Libft/Modules/GPGR/gl_funcs.hpp"
#include "../../Libft/Modules/GPGR/GpuShader.hpp"
#include "../../src/gpur/GpuTextureAtlas.hpp"
#include "../../src/camera/Camera.hpp"
#include "../../src/world/World.hpp"
#include "../../src/coordinates/WorldCoordinates.hpp"
#include <cstdint>
#include <vector>

class GpuRenderer
{
    public:
        struct MegaVertex
        {
            float    x, y, z;
            uint16_t tu, tv;
            uint32_t block_id;
            uint32_t face;
        };

    private:
    GpuShader       _world_shader;
    GpuShader       _sky_shader;
    GpuShader       _overlay_shader;
    GpuShader       _overlay_tex_shader;
    GpuShader       _tint_shader;
    GpuTextureAtlas _atlas;

    GLuint _mega_vao;
    GLuint _mega_vbo;
    GLuint _mega_ebo;
    size_t _mega_vbo_cap;
    size_t _mega_ebo_cap;
    std::vector<MegaVertex> _mega_verts;
    std::vector<uint32_t>   _mega_idxs;

    GLuint _water_vao;
    GLuint _water_ebo;
    size_t _water_ebo_cap;
    std::vector<uint32_t>   _water_idxs;

    GLuint _menu_tex;
    int    _menu_tex_w;
    int    _menu_tex_h;
    std::vector<uint8_t> _menu_rgba;

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
    GLint _u_tint_color;

    void build_mvp(float out[16], const Camera &camera,
        int width, int height, float near_z, float far_z) const;
    void draw_sky() const;
    void draw_crosshair() const;

    public:
        GpuRenderer();
        ~GpuRenderer();

        GpuRenderer(const GpuRenderer &);
        GpuRenderer &operator=(const GpuRenderer &);

        bool initialize(int width, int height);
        void destroy();
        void resize(int width, int height);
        void render(const Camera &camera, const World &world);
        void upload_overlay(const uint32_t *pixels, int w, int h);
        void draw_overlay();
        void draw_tint(float r, float g, float b, float a);
        void clear_screen();
        void upload_menu(const uint32_t *pixels, int w, int h);
        void draw_menu(float alpha);
        bool     is_ready() const;
        uint32_t gpu_mb_approx() const;
};

#endif
