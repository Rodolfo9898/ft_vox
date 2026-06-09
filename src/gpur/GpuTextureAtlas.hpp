#ifndef GPUR_TEXTURE_ATLAS_HPP
# define GPUR_TEXTURE_ATLAS_HPP

#ifndef GAME_USE_VOXEL_REGION_BACKEND
# define GAME_USE_VOXEL_REGION_BACKEND
#endif
#include <cstring>
#include <algorithm>
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include "../../Libft/Modules/Voxel/voxel_mesh.hpp"
#include "../../src/frame/RendererColor.hpp"
#include "../../src/gpur/gl_funcs.hpp"
#include "../../src/assets/TextureAtlas.hpp"

class GpuTextureAtlas
{
    GLuint _tex;
    bool   _loaded;

    public:
        GpuTextureAtlas() noexcept;
        GpuTextureAtlas(const GpuTextureAtlas &other) noexcept;
    ~GpuTextureAtlas() noexcept;
    GpuTextureAtlas &operator=(const GpuTextureAtlas &other) noexcept;


        bool upload(const TextureAtlas &atlas) noexcept;
        void bind(int unit) const noexcept;
        void destroy() noexcept;
        bool is_loaded() const noexcept;

        void fill_tile_uvs(const TextureAtlas &atlas,
            float uv_out[96 * 4]) const noexcept;
        void fill_fallback_colors(float colors_out[16 * 3]) const noexcept;
};

#endif
