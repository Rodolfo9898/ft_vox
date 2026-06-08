#ifndef GPUR_TEXTURE_ATLAS_HPP
# define GPUR_TEXTURE_ATLAS_HPP

#include "../../src/gpur/gl_funcs.hpp"
#include "../../src/assets/TextureAtlas.hpp"

class GpuTextureAtlas
{
    GLuint _tex;
    bool   _loaded;

    public:
        GpuTextureAtlas() noexcept : _tex(0), _loaded(false) {}
        ~GpuTextureAtlas() noexcept { destroy(); }

        GpuTextureAtlas(const GpuTextureAtlas &)            = delete;
        GpuTextureAtlas &operator=(const GpuTextureAtlas &) = delete;

        bool upload(const TextureAtlas &atlas) noexcept;
        void bind(int unit) const noexcept;
        void destroy() noexcept;
        bool is_loaded() const noexcept { return _loaded; }

        void fill_tile_uvs(const TextureAtlas &atlas,
            float uv_out[96 * 4]) const noexcept;
        void fill_fallback_colors(float colors_out[16 * 3]) const noexcept;
};

#endif
