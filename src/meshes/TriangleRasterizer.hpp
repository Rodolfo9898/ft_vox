#ifndef TRIANGLE_RASTERIZER_HPP
# define TRIANGLE_RASTERIZER_HPP

#include "../../src/geometry/ScreenVertex.hpp"
#include "../../Libft/Modules/DUMB/render_window.hpp"
#include <cstdint>
#include <vector>

class TriangleRasterizer
{
    public:
        TriangleRasterizer();
        TriangleRasterizer(const TriangleRasterizer &other);
        virtual ~TriangleRasterizer();
        TriangleRasterizer &operator=(const TriangleRasterizer &other);

        virtual void draw_triangle(ft_render_framebuffer &framebuffer,
            std::vector<double> &depth_buffer,
            const ScreenVertex vertices[3], uint32_t block_id,
            uint8_t face) const = 0;
};

#endif
