#ifndef GPU_WORLD_RENDERER_HPP
# define GPU_WORLD_RENDERER_HPP

#include "../../Libft/Modules/GPGR/GpuShader.hpp"
#include "../../Libft/Modules/GPGR/gl_funcs.hpp"
#include "../../src/gpur/GpuTextureAtlas.hpp"
#include "../../src/camera/Camera.hpp"
#include "../../src/world/World.hpp"
#include "../../src/frame/RenderCache.hpp"
#include "../../src/meshes/MeshCuller.hpp"
#ifndef GAME_USE_VOXEL_REGION_BACKEND
# define GAME_USE_VOXEL_REGION_BACKEND
#endif
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include "../../Libft/Modules/Voxel/voxel_mesh.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <vector>

struct MegaVertex
{
    float    x, y, z;
    uint16_t tu, tv;
    uint32_t block_id;
    uint32_t face;
};

class GpuWorldRenderer
{
    GpuShader       _world_shader;
    GpuTextureAtlas _atlas;

    GLuint  _mega_vao;
    GLuint  _mega_vbo;
    GLuint  _mega_ebo;
    size_t  _mega_vbo_cap;
    size_t  _mega_ebo_cap;
    std::vector<MegaVertex> _mega_verts;
    std::vector<uint32_t>   _mega_idxs;

    GLuint  _water_vao;
    GLuint  _water_ebo;
    size_t  _water_ebo_cap;
    std::vector<uint32_t>   _water_idxs;

    float   _tile_uvs[96 * 4];
    float   _fallback_colors[16 * 3];

    GLint   _u_mvp;
    GLint   _u_atlas;
    GLint   _u_atlas_loaded;
    GLint   _u_tile_uvs;
    GLint   _u_fallback;

    void setup_vao(GLuint vao, GLuint vbo, GLuint ebo) noexcept;
    void build_buffers(const Camera &camera, const World &world) noexcept;
    void upload_and_draw_opaque() noexcept;
    void upload_and_draw_water() noexcept;

    public:
        GpuWorldRenderer();
        GpuWorldRenderer(const GpuWorldRenderer &other);
        ~GpuWorldRenderer();
        GpuWorldRenderer &operator=(const GpuWorldRenderer &other);

        bool initialize() noexcept;
        void destroy() noexcept;

        void render(const Camera &camera,
            const World &world,
            const float  mvp[16]) noexcept;

        size_t gpu_bytes() const noexcept;
};

#endif
