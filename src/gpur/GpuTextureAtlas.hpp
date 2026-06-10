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
#include "../../Libft/Modules/GPGR/gl_funcs.hpp"
#include "../../src/assets/TextureAtlas.hpp"

class GpuTextureAtlas
{
    GLuint _tex;
    bool   _loaded;

    public:
        GpuTextureAtlas();
        GpuTextureAtlas(const GpuTextureAtlas &other);
    ~GpuTextureAtlas();
    GpuTextureAtlas &operator=(const GpuTextureAtlas &other);


        bool upload(const TextureAtlas &atlas);
        void bind(int unit) const;
        void destroy();
        bool is_loaded() const;

        void fill_tile_uvs(const TextureAtlas &atlas,
            float uv_out[96 * 4]) const;
        void fill_fallback_colors(float colors_out[16 * 3]) const;
};

#endif
