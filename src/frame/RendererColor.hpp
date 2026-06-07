#ifndef RENDERER_COLOR_HPP
# define RENDERER_COLOR_HPP

#include "../../Libft/Modules/Voxel/voxel.hpp"
#include "../../Libft/Modules/Voxel/voxel_mesh.hpp"
#include <cstdint>

class RendererColor
{
    private:
        static uint8_t shade_channel(uint8_t channel, double shade);

    public:
        RendererColor();
        RendererColor(const RendererColor &other);
        ~RendererColor();
        RendererColor &operator=(const RendererColor &other);

        static uint32_t rgba(uint8_t red, uint8_t green, uint8_t blue);
        static uint32_t block_color(uint32_t block_id, uint8_t face);
        static uint32_t shade_color(uint32_t color, double shade);
        static double face_shade(uint8_t face);
};

#endif
