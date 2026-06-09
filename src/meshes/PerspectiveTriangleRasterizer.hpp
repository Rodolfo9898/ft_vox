#ifndef PERSPECTIVE_TRIANGLE_RASTERIZER_HPP
# define PERSPECTIVE_TRIANGLE_RASTERIZER_HPP

#include "../../src/frame/RendererColor.hpp"

#include "../../src/assets/TextureAtlas.hpp"

#include "../../src/meshes/TriangleRasterizer.hpp"
#include <cmath>
#include <algorithm>

class PerspectiveTriangleRasterizer : public TriangleRasterizer
{
    public:
        PerspectiveTriangleRasterizer();
        PerspectiveTriangleRasterizer(const PerspectiveTriangleRasterizer &other);
        ~PerspectiveTriangleRasterizer();
        PerspectiveTriangleRasterizer &operator=(const PerspectiveTriangleRasterizer &other);

        void draw_triangle(ft_render_framebuffer &framebuffer,
            std::vector<double> &depth_buffer,
            const ScreenVertex vertices[3], uint32_t block_id,
            uint8_t face) const;

    private:
        static double edge(const ScreenVertex &a, const ScreenVertex &b,
            double x, double y);
        static double edge_step_x(const ScreenVertex &a,
            const ScreenVertex &b);
        static double edge_step_y(const ScreenVertex &a,
            const ScreenVertex &b);
};

#endif
