#ifndef GPU_WORLD_RENDERER_HPP
#define GPU_WORLD_RENDERER_HPP

#ifndef GAME_USE_VOXEL_REGION_BACKEND
#define GAME_USE_VOXEL_REGION_BACKEND
#endif
#include "../../Libft/Modules/GPGR/GpuShader.hpp"
#include "../../Libft/Modules/GPGR/gl_funcs.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include "../../src/assets/TextureAtlas.hpp"
#include "../../src/camera/Camera.hpp"
#include "../../src/frame/RenderCache.hpp"
#include "../../src/gpur/GlslLoader.hpp"
#include "../../src/gpur/GpuTextureAtlas.hpp"
#include "../../src/meshes/MeshCuller.hpp"
#include "../../src/world/World.hpp"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

class GpuWorldRenderer
{
  public:
    struct MegaVertex
    {
        float x, y, z;
        uint16_t tu, tv;
        uint32_t block_id;
        uint32_t face;
    };

    GpuWorldRenderer();
    GpuWorldRenderer(const GpuWorldRenderer &other);
    ~GpuWorldRenderer();
    GpuWorldRenderer &operator=(const GpuWorldRenderer &other);

    bool initialize(int width, int height, GLuint sky_vao, GLuint crosshair_vao,
                    GLuint crosshair_vbo, const std::string &shader_dir);
    void destroy();
    void resize(int width, int height);
    void render(const Camera &camera, const World &world);
    size_t gpu_bytes() const;

  private:
    GpuShader _world_shader;
    GpuShader _sky_shader;
    GpuShader _crosshair_shader;
    GpuTextureAtlas _atlas;

    GLuint _mega_vao;
    GLuint _mega_vbo;
    GLuint _mega_ebo;
    size_t _mega_vbo_cap;
    size_t _mega_ebo_cap;
    std::vector<MegaVertex> _mega_verts;
    std::vector<uint32_t> _mega_idxs;

    GLuint _water_vao;
    GLuint _water_ebo;
    size_t _water_ebo_cap;
    std::vector<uint32_t> _water_idxs;

    GLuint _sky_vao;
    GLuint _crosshair_vao;
    GLuint _crosshair_vbo;

    int _width;
    int _height;

    float _tile_uvs[96 * 4];
    float _fallback_colors[16 * 3];

    GLint _u_mvp;
    GLint _u_atlas;
    GLint _u_atlas_loaded;
    GLint _u_tile_uvs;
    GLint _u_fallback;
    GLint _u_sky_size;
    GLint _u_crosshair_color;

    static const float FOV_X;

    bool compile_shaders(const std::string &shader_dir);
    void cache_uniforms();
    void setup_world_vao();
    void setup_water_vao();
    void bind_mega_vertex_attribs();
    void upload_atlas_uniforms();
    void collect_geometry(const Camera &camera, const World &world);
    void add_chunk_vertices(const WorldChunk &wc, uint32_t base);
    void flush_solid(const float mvp[16]);
    void flush_water(const float mvp[16]);
    void build_view_matrix(float view[16], const Camera &camera) const;
    void build_proj_matrix(float proj[16], int width, int height, float near_z, float far_z) const;
    void build_mvp(float out[16], const Camera &camera, int width, int height, float near_z,
                   float far_z) const;
    void draw_sky() const;
    void draw_crosshair() const;

    static bool compile_shader_from_file(GpuShader &shader, const char *vert_path,
                                         const char *frag_path);
};

#endif
